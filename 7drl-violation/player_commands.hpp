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

#include "battery.hpp"
#include "basic_types.hpp"
#include "interaction_types.hpp"
#include "items.hpp"
#include "timeline.hpp"

// Maybe it should be called GameActions or GameState and probably hold these
// members not by reference, but fully own them
class PlayerInput
{
    BatteryManager battery;
    Timeline& time;
    InventoryManager items;
    std::vector<district_id_type>& travels;

    bool game_done = false;
public:
    PlayerInput(Timeline & time,
                std::vector<district_id_type> & travels) :
    time(time),
    travels(travels) {}

    auto is_done() const
    {
        return game_done;
    }

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

    auto get_id() const
    {
        return items.get_id();
    }

    auto start_id_check(WorldPosition const& pos)
    {
        auto id = items.get_id();
        time.add_id_check(id, pos);
    }

    auto purchase_fake_id()
    {
        // FIXME: Change price based on id type
        auto balance = pay_anonymously(3000);

        auto id = items.generate_id();

        items.set_id(id);

        return balance;
    }

    auto pay_for_charge(WorldPosition const& pos, int price)
    {
        pay_for_something(pos, price);
        battery.charge(BatteryManager::max_charge);
    }

    auto get_charge() const
    {
        return battery.get_charge();
    }

    auto toggle_fake_id()
    {
        items.toggle_fake();
    }

    auto travel(district_id_type district_id)
    {
        travels.push_back(district_id);
    }

    auto end_game()
    {
        game_done = true;
    }

    auto discharge(int amount = 1)
    {
        return battery.discharge(amount);
    }
};

#endif /* player_commands_h */
