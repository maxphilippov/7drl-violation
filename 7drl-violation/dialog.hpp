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

#include "id.hpp"
#include "interaction_types.hpp"
#include "player_commands.hpp"
#include "time.hpp"

// Constant declarations
auto police_officer_interaction()
{
    auto root = DialogNode {
        "Ma'am, can I check your id, please?",
        {
            {
                "Cooperate", {
                    "Thank you, this might take a moment", {}, {}
                },
            },
            {
                "Refuse", {
                    "Ma'am, you're acting very suspicious", {}, {}
                },
            }
        }, {}
    };

    return root;
}

auto phone_user_interface(WorldPosition const& location,
                          IDData const& data,
                          PlayerInput & input,
                          int turn_counter)
{
    std::ostringstream ss;

    ss << turns_to_hours(turn_counter) << " hours passed.";

    ss << "You're under id, " << data.name;

    auto root = DialogNode {
        ss.str(), {
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
                "Travel", {
                    "Initializing superflight feature",
                    {},
                    [&input, &location]() { input.travel(1); }
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
        "Your master is dead, the blood is on your hands\nHurry up, they are looking for a female android", {}
    };

    return root;
}

#endif /* dialog_h */
