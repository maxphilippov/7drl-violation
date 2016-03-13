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

#include "id.hpp"

class InventoryManager
{
    IDData realID;
    std::unique_ptr<IDData> fakeID; // Need a way to turn this on-off

    IDGenerator gen;

    bool use_fake = true;

    std::vector<int> player_posessions; // bloodpacks, ???
public:
    InventoryManager() :
    realID{1, "Mikoto", IDData::android, 5000},
    fakeID(nullptr) {}

    void change_ID(IDData d)
    {
        fakeID.reset(new IDData{d});
    }

    auto set_id(IDData id)
    {
        fakeID = std::make_unique<IDData>(id);
    }

    auto get_id() const
    {
        return (use_fake && fakeID != nullptr) ? *fakeID : realID;
    }

    auto toggle_fake()
    {
        use_fake = !use_fake;
    }

    auto generate_id()
    {
        // TODO: Generate balance
        return gen.next(3000);
    }

    auto pay(int amount)
    {
        realID.balance -= amount;

        return realID.balance;
    }
};


#endif /* items_h */
