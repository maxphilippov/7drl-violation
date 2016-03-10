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

#include "time.hpp"

struct DialogNode
{
    typedef std::vector<std::pair<std::string, DialogNode>> Replies;
    std::string message;
    Replies replies;
    std::function<void()> action;
};

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

auto phone_user_interface(int turn_counter)
{
    std::ostringstream ss;

    ss << turns_to_hours(turn_counter) << " hours passed";

    auto root = DialogNode {
        ss.str(), {
            {
                "Order tickets", {}
            },
            {
                "Call police", {}
            },
            {
                "Travel", {}
            },
            {
                "Quit", {}
            }
        }
    };

    return root;
}

#endif /* dialog_h */
