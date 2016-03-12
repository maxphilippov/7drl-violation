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
#include "interface.hpp"
#include "interaction_types.hpp"
#include "player_commands.hpp"

#include "police.hpp"
#include "city.hpp"
#include "time.hpp"

namespace utility
{
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
}

auto bar_interaction()
{
    auto root = DialogNode{
        "Bar is empty, no shady figures around", {}
    };

    return root;
}

auto clinic_interaction(WorldPosition const& loc)
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

auto station_travel_dialog(WorldPosition const& location,
                           IDData const& data,
                           PlayerInput & input,
                           std::vector<district_id_type> neighbour_districts,
                           int turn_counter)
{
    auto root = DialogNode {
        "Where to? It's one-way ticket",
        utility::travel_options(location, data, input, neighbour_districts, turn_counter)
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

    // FIXME: Write only fake ID and set connection type
    ss << "You're under id, " << data.name << ". Your connection is public.";

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
                // Adds a point of interest to police, if you do it twice with same ID
                // it will be recorded as violation
                "Call police", {}
            },
            {
                "Travel", station_travel_dialog(location, data, input, neighbour_districts, turn_counter)
            },
            {
                // FIXME: Change connection to another node // ? why
                "Change node", {}
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
        "Your master is dead, the blood is on your hands", {
            {
                "<Continue>", {
                    "Hurry up, they are looking for a female android", {}
                }
            }
        }
    };

    return root;
}

auto prison_dialog()
{
    auto root = DialogNode {
        "You can say that was close, but they got you", {
            {
                "<Continue>", {
                    "Bet that's gonna be hard to explain why you killed him",
                    {},
                    []() {
                        // end game
                    }
                }
            }
        }
    };

    return root;
}

auto death_dialog()
{
    auto root = DialogNode {
        "Well you're dead now, at least they couldn't get you", {
            {
                "<Continue>", {
                    "",
                    {},
                    []() {
                        // end game
                    }
                }
            }
        }
    };

    return root;
}

auto no_charge_dialog()
{
    auto root = DialogNode {
        "Your battery is done, you're not a human after all", {
            {
                "<Continue>", {
                    "You gotta recharge often, now they gonna get you",
                    {
                        {
                            "<Continue>", {
                                "Let's skip that part when they ask you why you did that",
                                {},
                                []() {
                                    // end game
                                }
                            }
                        }
                    }
                }
            }
        }
    };

    return root;
}

auto welcome_to_a_new_life_dialog(std::string const& name)
{
    std::ostringstream ss;

    ss << "Welcome to a new life, " << name;

    auto root = DialogNode {
        ss.str(), {
            {
                "<Continue>", {
                    "Anyway, why you did this?",
                    {},
                    []() {
                        // end game
                    }
                }
            }
        }
    };

    return root;
}

auto outro_dialog(MapSize const& screen)
{
    auto root = DialogNode {
        "You slipped away, now you're safe", {
            {
                "<Continue>", {
                    "", {},
                    [&screen]() {
                        auto name = prompt("What's your name: ");

                        render_dialog(screen, welcome_to_a_new_life_dialog(name));
                    }
                }
            }
        }
    };
    
    return root;
}

#endif /* dialog_h */
