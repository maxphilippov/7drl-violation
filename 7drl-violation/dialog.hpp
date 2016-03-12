//
//  dialog.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef dialog_h
#define dialog_h

#include <string>
#include <sstream>
#include <vector>

#include "basic_types.hpp"
#include "id.hpp"
#include "interaction_types.hpp"
#include "player_commands.hpp"

#include "police.hpp"
#include "city.hpp"
#include "time.hpp"

auto bar_interaction()
{
    auto root = DialogNode{
        "Bar is empty, no shady figures around", {}
    };

    return root;
}

auto clinic_interaction()
{
    auto root = DialogNode{
        "You walk down the hall and see a bloodpack lying right under your feet", {
            {
                "Steal a bloodpack", {
                    "You've acquired a bloodpack, useful to fake injuries", {}
                }
            }
        }
    };

    return root;
}

auto repair_station_interaction(IDData const& id)
{
    auto root = DialogNode{
        "You are warmly welcomed", {
            {
                "Pay for recharge", {}
            },
            {
                "Pay for repair", {}
            }
        }
    };

    return root;
}

auto cantina_interaction(IDData const& id)
{
    auto root = DialogNode{
        "You are warmly welcomed", {
            {
                "Order a meal", {}
            },
            {
                "Chat with people", {}
            }
        }
    };

    return root;
}

// Constant declarations
auto police_officer_interaction(WorldPosition const& location,
                                IDData const& id,
                                PoliceManager & police)
{
    auto root = DialogNode {
        "Ma'am, can I check your id, please?",
        {
            {
                "Cooperate", {
                    "Thank you, this might take a moment, don't walk too far away", {},
                    [&location, &police, &id]() {
                        police.check_crime_history(id.id, location);
                        // OK, cool, what's next?
                    }
                },
            },
            {
                "Refuse", {
                    "Ma'am, you're acting very suspicious. I think I got to take you back to police station\n<he gets wristbands>",
                    {
                        { "Punch him back", {
                            "<You stunned poor guy>", {}
                        } },
                        { "Punch him back (excessive)", {
                            "<You killed poor guy>", {}
                        } },
                        { "Run away", {} },
                        { "Run away on steroids", {} }
                    }
                },
            }
        }, {}
    };

    return root;
}

auto travel_options(WorldPosition const& location,
                          IDData const& data,
                          PlayerInput & input,
                          std::vector<district_id_type> const& ids,
                          int turn_counter)
{
    auto replies = DialogNode::Replies{};
    for(auto const& id: ids) {
        std::ostringstream ss;

        ss << "District #" << id;
        replies.push_back({ ss.str(), {
            "", {},
            [&input, &location, id]() {
                input.pay_for_something(location, 1000);
                input.travel(id);
            }
        } } );
    }

    return replies;
}

auto station_travel_dialog(WorldPosition const& location,
                           IDData const& data,
                           PlayerInput & input,
                           std::vector<district_id_type> neighbour_districts,
                           int turn_counter)
{
    auto root = DialogNode {
        "", travel_options(location, data, input, neighbour_districts, turn_counter)
    };

    return root;
}

auto phone_user_interface(WorldPosition const& location,
                          IDData const& data,
                          PlayerInput & input,
                          std::vector<district_id_type> neighbour_districts,
                          int turn_counter)
{
    std::ostringstream ss;

    ss.precision(1);

    ss << turns_to_hours(turn_counter) << " hours passed.";

    ss << "You're under id, " << data.name;

    auto travel_options = DialogNode::Replies{};

    auto root = DialogNode {
        ss.str(), {
            {
                "Start ID check", {
                    "You're about to submit your id for checking",
                    {
                        {
                            "Submit", {
                                "",
                                {},
                                [&input, &location]() { input.start_id_check(location); }
                            }
                        },
                        {
                            "Abort", {
                                "",
                                {}
                            }
                        }
                    }
                }
            },
            {
                "Order tickets", {
                    "Ordering tickets",
                    {},
                    [&input, &location]() { input.pay_for_something(location, 3000); }
                }
            },
            {
                "Call police", {}
            },
            {
                "Travel", station_travel_dialog(location, data, input, neighbour_districts, turn_counter)
            },
            {
                "Quit", {}
            }
        }
    };

    return root;
}

auto intro_dialog()
{
    auto root = DialogNode {
        "Your master is dead, the blood is on your hands\nHurry up, they are looking for a female android", {}
    };

    return root;
}

#endif /* dialog_h */
