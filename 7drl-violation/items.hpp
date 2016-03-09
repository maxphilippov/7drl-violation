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
#include "timeline.hpp"

class InventoryManager
{
    IDData realID;
    // Doesn't work due to movability
//    std::unique_ptr<IDData> fakeID; // Need a way to turn this on-off

    std::vector<int> player_posessions; // bloodpacks, ???
public:
    InventoryManager() :
    realID{"Mikoto", IDData::android, 5000} {}
//    {
//        fakeID.reset(nullptr);
//    }
//
//    void change_ID(IDData d)
//    {
//        fakeID.reset(new IDData{d});
//    }

    IDData get_id() const
    {
        return realID;
    }

    auto pay(int amount)
    {
        realID.balance -= amount;

        return realID.balance;
    }
};


#endif /* items_h */
