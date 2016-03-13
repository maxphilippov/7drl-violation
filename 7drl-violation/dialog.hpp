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

        replies.push_back({ "Quit", { "Have a nice day, miss" } });

        return replies;
    }
}

auto bar_interaction(GameState & state,
                     std::vector<PoliceAlert> & police_alerts,
                     WorldPosition const& loc)
{
    auto root = DialogNode{
        "Bar is empty, no shady figures around", {
            {
                "Look for someone who can get you a fake ID", {
                    "You've found a guy, he offers a random id for 3000", {
                        {
                            "Purchase <this will replace your current fake ID>", {
                                "You paid for ID",
                                {},
                                [&state]() {
                                    state.purchase_fake_id();
                                }
                            }
                        },
                        {
                            "Leave", {
                                "The guy looks confused", {}
                            }
                        }
                    }
                }
            }
        }
    };

    return root;
}

auto clinic_interaction(GameState & state,
                        std::vector<PoliceAlert> & police_alerts,
                        WorldPosition const& loc)
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
                                std::vector<PoliceAlert> & police_alerts,
                                WorldPosition const& loc)
{
    auto id = state.get_id();

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
                                [&id, &police_alerts, &state, &loc, full_charge_price]() {
                                    if (id.is_human()) {
                                        police_alerts.push_back({
                                            id.id,
                                            loc,
                                            30
                                        });
                                    }
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

auto conflict_dialog(GameState const& state,
                     std::vector<PoliceAlert> & police_alerts,
                     WorldPosition const& loc,
                     bool android_behaviour,
                     int witness_count = 1)
{
    auto id = state.get_id();
    auto conflict_reaction = DialogNode {"", {}};

    if (witness_count == 0) {
        conflict_reaction.message = "You successfully escaped";
    } else {
        conflict_reaction.message = "Someone saw you escaping, they'll call the police";
        auto violation_level = (id.is_human() && android_behaviour) ? 200 : 100;
        conflict_reaction.action = [&police_alerts, &loc, &id, violation_level]() {
            police_alerts.push_back({id.id, loc, violation_level});
        };
    }

    return conflict_reaction;
}

auto po_inspection(GameState & state,
                   PoliceManager & police,
                   std::vector<PoliceAlert> & police_alerts,
                   WorldPosition const& loc,
                   identity_id_type id)
{
    auto root = DialogNode{};

    auto violation_level = police.check_crime_history(id, loc);

    if (violation_level < 50) {
        root.message = "Ok, you're good to go, thank you for understanding";
    } else {
        // FIXME:
        root.message = "Sorry, ma'am, but I think you got to go with us";
        root.action = [&state]() { state.to_jail(); };
    }

    return root;
}

// Constant declarations
auto police_officer_interaction(GameState & state,
                                PoliceManager & police,
                                std::vector<PoliceAlert> & police_alerts,
                                WorldPosition const& loc,
                                // Cause I removed violence options it's always gonna
                                // be a PO who will be a witness
                                int witness_count = 1)
{

    auto id = state.get_id();
    auto root = DialogNode {
        "Ma'am, can I check your id, please?",
        {
            {
                "Cooperate", {
                    "Thank you, this might take a moment, don't walk too far away", {
                        {
                            "<Continue>",
                            po_inspection(state, police, police_alerts, loc, id.id)
                        }
                    }
                },
            },
            {
                "Refuse", {
                    "Ma'am, you're acting very suspicious. I think I got to take you back to police station\n<he gets wristbands>",
                    {
                        // Removed violence
                        {
                            "Run away",
                            conflict_dialog(state, police_alerts, loc, false, witness_count)
                        },
                        {
                            "Run away on steroids", {
                                "", {
                                    {
                                        "<Continue>",
                                        conflict_dialog(state, police_alerts, loc, true, witness_count)
                                    }
                                }, [&state]() { state.discharge(10); }
                            }
                        }
                    }
                },
            }
        }, {}
    };

    return root;
}


auto leaving_dialog(GameState & state)
{
    auto replies = DialogNode::Replies {};

    auto root = DialogNode{
        "Can I help you ma'am?", {
            {
                "Yes, please, the train is waiting for me", {
                    "Ok, let me see... Yeah, you're perfectly fine to go", {
                        {
                            "Thank you", {
                                "Hope you enjoy the trip", {
                                    {
                                        "You bet I will", {
                                            "Have a nice day!", {}, [&state]() { state.leave_on_train(); }
                                        }
                                    }
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

auto station_travel_dialog(GameState & state,
                           WorldPosition const& loc,
                           std::vector<district_id_type> const& neighbour_districts,
                           district_id_type district,
                           int turn_counter)
{
    auto data = state.get_id();
    auto root = DialogNode{
        "Where to? It's one-way ticket",
        utility::travel_options(loc, state, neighbour_districts, turn_counter)
    };

    if (district == 4) {
        root.replies.push_back({
            "Get a ticket for a train", {
                "To get on a train you'll need to pass a full ID check",
                {},
                [&state, &loc]() { state.purchase_train_ticket(loc); }

            }
        });

        if (state.has_ticket(data.id)) {
            root.replies.push_back({
                "Start ID check", {
                    "Ok, you will have to wait a bit, we will call you",
                    {},
                    [&state, &loc]() { state.start_id_check(loc); }

                }
            });
            if (state.has_id_checked(data.id)) {
                root.replies.push_back({
                    "<Train awaits>", leaving_dialog(state)
                });
            }
        }
    }

    return root;
}


auto nothing_to_do_dialog()
{
    return DialogNode{ "There's nothing to do here" };
}

auto tile_to_interaction(MapTile tile,
                         WorldPosition &loc,
                         GameState & state,
                         std::vector<PoliceAlert> & police_alerts,
                         std::vector<district_id_type> const& neighbour_districts,
                         district_id_type district,
                         int turn_counter)
{
    DialogNode r;
    switch(tile) {
        case Bar:
            r = bar_interaction(state, police_alerts, loc);
            break;
        case Clinic:
            r = clinic_interaction(state, police_alerts, loc);
            break;
        case Repairs:
            r = repair_station_interaction(state, police_alerts, loc);
            break;
        case Station:
            r = station_travel_dialog(state, loc, neighbour_districts, district, turn_counter);
            break;
        default:
            r = nothing_to_do_dialog();
            break;
    }
    return r;
}

auto phone_user_interface(GameState & state,
                          std::vector<PoliceAlert> & police_alerts,
                          WorldPosition const& loc,
                          std::vector<district_id_type> neighbour_districts,
                          int turn_counter)
{
    auto data = state.get_id();
    auto battery_charge = state.get_charge_in_hours();
    auto battery_percent = state.get_charge();
    std::ostringstream ss;

    ss.precision(2);

    auto dh = turns_to_days_hours(turn_counter);
    ss << "d" << dh.first << "h" << dh.second;

    ss << " Battery: (" << battery_percent << "%% left (" << battery_charge << " hours)";

    ss << " ID: " << data.name << "," << IDData::type_to_string(data.type);

    ss << " Balance: " << data.balance;

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
                // Adds a point of interest to police for anonymous crime
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
                    "Find a bar, purchase a fake ID with the money you have", {
                        {
                            "<Continue>", {
                                "Then get to district #5 and purchase a ticket for a train", {
                                    {
                                        "<Continue>", {
                                            "Now hurry up, they are looking for a female android", {
                                                {
                                                    "<Continue>", {
                                                        "When in doubt press ?"
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    };
    // YEP, it's a so called "arrow-syntax"

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
                    "You gotta recharge often, now they gonna get you", {
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

#endif /* dialog_h */
