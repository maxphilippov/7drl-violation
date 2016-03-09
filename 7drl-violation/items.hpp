//
//  items.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef items_h
#define items_h

#include <memory>

#include "id.hpp"
#include "timeline.hpp"

class InventoryManager
{
    IDData realID;
    //std::unique_ptr<IDData> fakeID; // Need a way to turn this on-off
    
    std::vector<int> player_posessions; // bloodpacks, ???
public:
    InventoryManager():
    realID{"Mikoto", IDData::android, 5000} {}
//    fakeID(nullptr) {}

//    void change_ID(IDData d)
//    {
//        fakeID.reset(new IDData{d});
//    }
//    
//    void purchase(Timeline& time)
//    {
//        // TODO: How do we handle pointer here?
//        time.add_purchase_check(realID, Hours(1.5f));
//    }

};


#endif /* items_h */
