//
//  game_state.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef game_state_h
#define game_state_h

#include <sstream>
#include <unordered_set>
#include <vector>

#include "battery.hpp"
#include "basic_types.hpp"
#include "interaction_types.hpp"
#include "items.hpp"
#include "timeline.hpp"

// Maybe it should be called GameActions or GameState and probably hold these
// members not by reference, but fully own them
class GameState
{
    BatteryManager battery;
    Timeline& time;
    InventoryManager items;
    std::vector<district_id_type>& travels;

    bool game_done = false;
    bool win = false;

    // Purchased tickets
    std::unordered_set<identity_id_type> tickets;

    std::unordered_set<identity_id_type> checked_ids;
public:
    GameState(Timeline & time,
                std::vector<district_id_type> & travels) :
    time(time),
    travels(travels) {}

    auto is_won() const
    {
        return win;
    }

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

    auto get_fake_id() const
    {
return items.get_fake_id();
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

    auto discharge(int amount = 1)
    {
        return battery.discharge(amount);
    }

    auto purchase_train_ticket(WorldPosition const& pos)
    {
        auto id = get_id();
        // FIXME: if id == 0 You're fked
        pay_for_something(pos, 7000);
        tickets.insert(id.id);
    }

    auto leave_on_train()
    {
        // You can't buy a ticket on your
        auto id = get_fake_id();
        if (tickets.count(id.id)) {
            win = true;
        }
    }

    auto end_game()
    {
        game_done = true;
    }
};

#endif /* game_state_h */
