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

#include "id.hpp"
#include "position.hpp"

struct DialogNode
{
    typedef std::vector<std::pair<std::string, DialogNode>> Replies;
    std::string message;
    Replies replies;
    std::function<void()> action = []() {};
};

struct DialogData
{
    DialogNode root;
};

struct TravelData
{
    int destination_id;
};

struct PoliceAlert
{
    IDData::id_type id;
    WorldPosition pos;
    int violation_level;
};

namespace police_alerts
{

auto purchase_balance_alert(IDData::id_type id, WorldPosition const& pos)
{
    return PoliceAlert {
        id, pos, 3
    };
}

auto attempt_to_escape(IDData::id_type id, WorldPosition const& pos)
{
    return PoliceAlert {
        id, pos, 20
    };
}

}

#endif /* interaction_types_h */
