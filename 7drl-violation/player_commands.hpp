//
//  player_commands.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef player_commands_h
#define player_commands_h

#include <sstream>
#include <vector>

#include "interaction_types.hpp"
#include "items.hpp"
#include "timeline.hpp"

// Maybe it should be called GameActions or something
class PlayerInput
{
    Timeline& time;
    InventoryManager& items;
    std::vector<TravelData>& travels;
public:
    PlayerInput(Timeline & time,
                InventoryManager & items,
                std::vector<TravelData> & travels) :
    time(time),
    items(items),
    travels(travels) {}

    auto pay_for_something(WorldPosition const& pos,
                           int price)
    {
        auto id = items.get_id();
        auto balance = items.pay(price);
        time.add_purchase_check(id, pos, price, Hours(1.5f));

        return balance;
    }

    auto pay_anonymously(int price)
    {
        auto balance = items.pay(price);

        return balance;
    }

    auto purchase_fake_id() {
        // FIXME: Change price based on id type
        auto balance = pay_anonymously(3000);
    }

    auto travel(int district_id)
    {
        travels.push_back({ district_id });
    }
};

#endif /* player_commands_h */
