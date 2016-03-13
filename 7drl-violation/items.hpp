//
//  items.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef items_h
#define items_h

#include <memory>
#include <vector>

#include "random.hpp"

#include "id.hpp"

class InventoryManager
{
    IDData realID;
    std::unique_ptr<IDData> fakeID; // Need a way to turn this on-off

    IDGenerator gen;

    bool use_fake = true;
public:
    InventoryManager() :
    realID{player_real_identity},
    fakeID(nullptr) {}

    auto set_id(IDData id)
    {
        fakeID = std::make_unique<IDData>(id);
    }

    auto get_fake_id() const
    {
        return *fakeID;
    }

    auto get_id() const
    {
        return (use_fake && fakeID != nullptr) ? get_fake_id() : realID;
    }

    auto toggle_fake()
    {
        use_fake = !use_fake;
    }

    auto generate_id()
    {
        auto balance = generate_random_int(1000, 3000);
        return gen.next(balance);
    }

    auto make_money(int amount)
    {
        auto &balance = (use_fake && fakeID != nullptr)? fakeID->balance : realID.balance;
        balance += amount;

        return balance;
    }

    auto pay(int amount)
    {
        auto &balance = (use_fake && fakeID != nullptr)? fakeID->balance : realID.balance;
        balance -= amount;

        return balance;
    }

    auto anonymous_pay(int amount)
    {
        realID.balance -= amount;

        return realID.balance;
    }
};


#endif /* items_h */
