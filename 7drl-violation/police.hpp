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

// PO - Police officer

struct ActivatedPO
{
    CollisionManager::id_type collision_id;
};

struct PatrolMission
{
    unsigned int active_po_id;
    Position point_to_check;
};

struct CriminalRecord
{
    WorldPosition last_known_location; // should always be present
    // BITMASK MB?
    int violation_level;
};

class PoliceManager
{
    static const int spawn_interval = 2; // FIXME: 12;

    typedef int violation_level;
    typedef IDData::id_type id_name;
    // I don't need a fast lookup actually cause getting one value
    // out of this map is performed only when a PO checks your ID
    std::map<id_name, CriminalRecord> criminal_records;

    std::vector<CollisionManager::id_type> cops_on_the_street;

    std::vector<PatrolMission> on_patrol_mission;

    std::vector<WorldPosition> points_of_interest;
public:
    void update(Bounds const& simulation_bound,
                CollisionManager& collisions,
                int turn_count)
    {
        for(auto const& r: criminal_records) {
            // Decide which district holds the most amount of criminals
        }

        if (turn_count % spawn_interval == 0) {
            srand(static_cast<unsigned int>(time(0) * turn_count));
            auto w = simulation_bound.maxx - simulation_bound.minx;
            auto h = simulation_bound.maxy - simulation_bound.miny;
            // spawn new PO near some point of interest
            // FIXME: don't spawn in player vision range
            auto x = simulation_bound.minx + rand() % w;
            auto y = simulation_bound.miny + rand() % h;
            auto pos = Position{ x, y };

            auto missionTarget = Position { x + 5, y + 5 };
            auto id = collisions.add_moving_entity(pos);
            auto mission = PatrolMission{
                static_cast<unsigned int>(id),
                missionTarget
            };
            cops_on_the_street.push_back(id);
            on_patrol_mission.push_back(mission);
        }

        auto random_velocities = std::vector<Velocity>(cops_on_the_street.size());

        for(auto i: cops_on_the_street) {
            // FIXME: proper generation
            srand(i * turn_count);
            auto v = Velocity{rand() % 3 - 1, rand() % 3 - 1};
            collisions.change_velocity(i, v);
        }
    }
    
    void record_crime(id_name const& id, WorldPosition pos, violation_level level)
    {
        auto it = criminal_records.find(id);

        if (it != std::end(criminal_records)) {
            it->second.violation_level += level;
            // FIXME: Pass district id
            it->second.last_known_location = pos;
        }
    }
    
    auto check_crime_history(id_name const& id) const
    {
        return criminal_records.at(id);
    }
};


#endif /* police_h */
