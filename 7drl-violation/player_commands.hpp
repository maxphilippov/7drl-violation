//
//  player_commands.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef player_commands_h
#define player_commands_h

#include <sstream>

#include "items.hpp"
#include "id.hpp"
#include "timeline.hpp"

// Maybe create it every game loop and pass everyting in?
class PlayerInput
{
public:
    auto pay_for_something(Timeline & time,
                  InventoryManager & items,
                  Position const& pos,
                  int price)
    {
        auto id = items.get_id();
        auto balance = items.pay(price);
        time.add_purchase_check(id, pos, price, Hours(1.5f));

        return balance;
    }

    auto purchase_charge(Timeline & time,
                         InventoryManager & items,
                         Position const& pos,
                         int price)
    {
        return pay_for_something(time, items, pos, price);
    }

    auto travel(int district_id)
    {

    }
};

#endif /* player_commands_h */
