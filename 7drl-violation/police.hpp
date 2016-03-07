//
//  police.hpp
//  7drl-violation
//
//  Created by Max Philippov on 07/03/16.
//  Copyright © 2016 AGNMTLT. All rights reserved.
//

#ifndef police_h
#define police_h

#include <vector>

#include "position.hpp"
#include "collisions.hpp"

class PoliceManager
{
    std::vector<unsigned long> cops_on_the_street;
    
    int turn_jumper = 0;
public:
    void update(Position const& player, CollisionManager& collisions)
    {
        // Generate a new PO
        // TODO: add some check to that
        if (turn_jumper % 5 == 0 ) {
            auto id = collisions.add_moving_entity(Position{0, 0});
            cops_on_the_street.push_back(id);
        }
        
        if (turn_jumper % 2 == 0) {
            for(auto i: cops_on_the_street) {
                collisions.change_velocity(i, Velocity{1,1});
            }
        }
        ++turn_jumper;
    }
};


#endif /* police_h */
