//
//  dialog.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef dialog_h
#define dialog_h

#include <map>
#include <string>

struct DialogNode
{
    typedef std::map<std::string, DialogNode> Replies;
    std::string message;
    Replies replies;
};

// Constant declarations
auto police_officer_interaction()
{
    auto root = DialogNode {
        "Ma'am, can I check your id, please?",
        DialogNode::Replies {
            {
                "Cooperate", DialogNode {
                    "Thank you, this might take a moment", DialogNode::Replies {}
                }
            }
        }
    };

    return root;
}

#endif /* dialog_h */
