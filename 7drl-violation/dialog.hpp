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
#include "interface.hpp"
#include "interaction_types.hpp"
#include "game_state.hpp"

#include "police.hpp"
#include "city.hpp"
#include "time.hpp"

namespace utility
{
    auto travel_options(WorldPosition const& location,
                        GameState & state,
                        std::vector<district_id_type> const& ids,
                        int turn_counter)
    {
        auto replies = DialogNode::Replies{};
        for(auto const& id: ids) {
            std::ostringstream ss;

            ss << "District #" << id;
            replies.push_back({ ss.str(), {
                "", {},
                [&state, &location, id]() {
                    state.pay_for_something(location, 1000);
                    state.travel(id);
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

auto repair_station_interaction(GameState & state,
                                WorldPosition const& loc)
{
    std::ostringstream ss;

    auto full_charge_price = 500;

    ss << "Full charge will cost you " << full_charge_price;

    auto root = DialogNode{
        "You are warmly welcomed", {
            {
                "Pay for recharge", {
                    ss.str(), {
                        {
                            "Pay", {
                                "",
                                {},
                                [&state, &loc, full_charge_price]() {
                                    state.pay_for_charge(loc, full_charge_price);
                                }
                            }
                        },
                        {
                            "No, thanks", {
                                "Have a nice day, miss!",
                                {}
                            }
                        }
                    }
                }
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
auto police_officer_interaction(GameState & state,
                                PoliceManager & police,
                                WorldPosition const& location)
{
    auto id = state.get_id();
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
                            "<You killed poor guy>", {}, [&state]() { state.discharge(20); }
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

auto station_travel_dialog(GameState & state,
                           WorldPosition const& location,
                           std::vector<district_id_type> neighbour_districts,
                           int turn_counter)
{
    auto data = state.get_id();
    auto root = DialogNode {
        "Where to? It's one-way ticket",
        utility::travel_options(location, state, neighbour_districts, turn_counter)
    };

    return root;
}

auto phone_user_interface(GameState & state,
                          std::vector<PoliceAlert> & police_alerts,
                          WorldPosition const& loc,
                          std::vector<district_id_type> neighbour_districts,
                          int turn_counter)
{
    auto data = state.get_id();
    auto battery_charge = state.get_charge();
    std::ostringstream ss;

    ss.precision(1);

    ss << turns_to_hours(turn_counter) << " hours passed.";

    ss << "Battery: " << battery_charge << "%";

    // FIXME: Write only fake ID and set connection type
    ss << "Using a fake id: " << data.name << ". Your connection is public.";

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
                                [&state, &loc]() { state.start_id_check(loc); }
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
                    "To get on a train you'll need to pass a full ID check",
                    {},
                    [&state, &loc]() { state.pay_for_something(loc, 3000); }
                }
            },
            {
                // Adds a point of interest to police, if you do it twice with same ID
                // it will be recorded as violation
                "Call police", {
                    // FIXME:
                    "", {}, [&police_alerts, &loc]() {
                        police_alerts.push_back({0, loc, 3});
                    }
                }
            },
            {
                "Toggle fake ID usage", {
                    "", {}, [&state]() { state.toggle_fake_id(); }
                }
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
                    "Hurry up, they are looking for a female android", {
                        {
                            "<Continue>", {
                                "When in doubt press ?", {}
                            }
                        }
                    }
                }
            }
        }
    };

    return root;
}

auto prison_dialog(GameState & state)
{
    auto root = DialogNode {
        "You can say that was close, but they got you", {
            {
                "<Continue>", {
                    "Bet that's gonna be hard to explain why you killed him",
                    {},
                    [&state]() {
                        state.end_game();
                    }
                }
            }
        }
    };

    return root;
}

auto death_dialog(GameState & state)
{
    auto root = DialogNode {
        "Well you're dead now, at least they couldn't get you", {
            {
                "<Continue>", {
                    "",
                    {},
                    [&state]() {
                        state.end_game();
                    }
                }
            }
        }
    };

    return root;
}

auto no_charge_dialog(GameState & state)
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
                                [&state]() {
                                    state.end_game();
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

auto welcome_to_a_new_life_dialog(GameState & state, std::string const& name)
{
    std::ostringstream ss;

    ss << "Welcome to a new life, " << name;

    auto root = DialogNode {
        ss.str(), {
            {
                "<Continue>", {
                    "Anyway, why you did this?",
                    {},
                    [&state]() {
                        state.end_game();
                    }
                }
            }
        }
    };

    return root;
}

auto outro_dialog(GameState & state, MapSize const& screen)
{
    auto root = DialogNode {
        "You slipped away, now you're safe", {
            {
                "<Continue>", {
                    "", {},
                    [&state, &screen]() {
                        auto name = prompt("What's your name: ");

                        render_dialog(screen, welcome_to_a_new_life_dialog(state, name));
                    }
                }
            }
        }
    };
    
    return root;
}

auto nothing_to_do_dialog()
{
    auto root = DialogNode {
        "There's nothing to do here", {}
    };
    
    return root;
}

auto tile_to_interaction(MapTile tile,
                         WorldPosition &loc,
                         GameState & state,
                         std::vector<district_id_type> neighbour_districts,
                         int turn_counter)
{
    DialogNode r;
    switch(tile) {
        case Bar:
            r = bar_interaction();
            break;
        case Clinic:
            r = clinic_interaction(loc);
            break;
        case Repairs:
            r = repair_station_interaction(state, loc);
            break;
        case Station:
            r = station_travel_dialog(state, loc, neighbour_districts, turn_counter);
        default:
            r = nothing_to_do_dialog();
            break;
    }
    return r;
}

#endif /* dialog_h */
