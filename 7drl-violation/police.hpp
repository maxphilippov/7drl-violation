//
//  police.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
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

struct CriminalRecord
{
    WorldPosition last_known_location; // should always be present
    // BITMASK MB?
    int violation_level;
};

class PoliceManager
{
    static const int spawn_interval = 12;

    typedef int violation_level;
    typedef IDData::id_type id_name;
    // I don't need a fast lookup actually cause getting one value
    // out of this map is performed only when a PO checks your ID
    std::map<id_name, CriminalRecord> criminal_records;

    std::vector<CollisionManager::id_type> cops_on_the_street;
public:
    void update(Position const& player, CollisionManager& collisions, int turn_count)
    {
        for(auto const& r: criminal_records) {
            // Decide which district holds the most amount of criminals
        }

        if (turn_count % spawn_interval == 0) {
            // spawn new PO near some point of interest
            auto pos = Position{ 0, 0 };
            auto id = collisions.add_moving_entity(pos);
            cops_on_the_street.push_back(id);
        }

        auto random_velocities = std::vector<Velocity>(cops_on_the_street.size());

        for(auto i: cops_on_the_street) {
            // FIXME: proper generation
            srand(static_cast<unsigned int>(i * time(0) * turn_count));
            auto v = Velocity{rand() % 3 - 1, rand() % 3 - 1};
            collisions.change_velocity(i, v);
        }
    }
    
    void record_crime(id_name const& id, Position pos, violation_level level)
    {
        auto it = criminal_records.find(id);

        if (it != std::end(criminal_records)) {
            it->second.violation_level += level;
            // FIXME: Pass district id
            it->second.last_known_location = WorldPosition{ 0, pos };
        }
    }
    
    auto check_crime_history(id_name const& id) const
    {
        return criminal_records.at(id);
    }
};


#endif /* police_h */
