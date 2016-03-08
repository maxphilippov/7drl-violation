//
//  police.hpp
//  7drl-violation
//
//  Created by Max Philippov on 07/03/16.
//  Copyright © 2016 AGNMTLT. All rights reserved.
//

#ifndef police_h
#define police_h

#include <map>
#include <string>
#include <vector>

#include "collisions.hpp"
#include "id.hpp"
#include "position.hpp"

struct ActivatedPO
{
    CollisionManager::id_type collision_id;
};

class CriminalRecord
{
    WorldPosition last_known_location; // should always be present
    int violation_level;
};

class PoliceManager
{
    typedef int violation_level;
    typedef IDData::id_type id_name;
    // I don't need a fast lookup actually cause getting one value
    // out of this map is performed only when a PO checks your ID
    std::map<id_name, CriminalRecord> criminal_records;

    std::vector<CollisionManager::id_type> cops_on_the_street;
public:
    void update(Position const& player, CollisionManager& collisions)
    {
        for(auto const& r: criminal_records) {
            // Decide which district holds the most amount of criminals
        }
        
        // Generate a new PO
        // TODO: add some check to that
//            auto id = collisions.add_moving_entity(Position{0, 0});
//            cops_on_the_street.push_back(id);
//            for(auto i: cops_on_the_street) {
//                collisions.change_velocity(i, Velocity{1,1});
//            }
    }
    
    void record_crime(id_name const& id, Position* pos, violation_level level)
    {
        // criminal_records.at(id) += level;
    }
    
    auto check_crime_history(id_name const& id) const
    {
        return criminal_records.at(id);
    }
};


#endif /* police_h */
