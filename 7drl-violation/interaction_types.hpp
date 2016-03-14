//
//  dialog_node.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef interaction_types_h
#define interaction_types_h

#include <functional>
#include <string>
#include <vector>

#include "basic_types.hpp"
#include "position.hpp"

struct DialogNode
{
    typedef std::vector<std::pair<std::string, DialogNode>> Replies;
    std::string message;
    Replies replies;
    std::function<void()> action;
    DialogNode(std::string message = "",
               Replies replies = {},
               std::function<void()> action = []() {}) :
    message(message),
    replies(replies),
    action(action) {}
};

struct PoliceAlert
{
    identity_id_type id;
    WorldPosition pos;
    int violation_level;
};

namespace police_alerts
{
    auto purchase_balance_alert(identity_id_type id, WorldPosition const& pos)
    {
        return PoliceAlert {
            id, pos, 60
        };
    }

    auto attempt_to_escape(identity_id_type id, WorldPosition const& pos)
    {
        return PoliceAlert {
            id, pos, 200
        };
    }

    auto android_murder(identity_id_type id, WorldPosition const& pos)
    {
        return PoliceAlert {
            id, pos, 1000
        };
    }
}

#endif /* interaction_types_h */
