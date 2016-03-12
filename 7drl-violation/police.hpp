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

#include "random.hpp"

#include "time.hpp"
#include "collisions.hpp"
#include "id.hpp"
#include "position.hpp"
#include "interaction_types.hpp"

// PO - Police officer

struct ActivatedPO
{
    PhysicalData::id_type collision_id;
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
    static const int spawn_interval = 12;

    typedef int violation_level;
    typedef IDData::id_type id_name;
    // I don't need a fast lookup actually cause getting one value
    // out of this map is performed only when a PO checks your ID
    std::map<id_name, CriminalRecord> criminal_records;

    std::vector<PhysicalData::id_type> cops_on_the_street;

    std::vector<PatrolMission> on_patrol_mission;

    std::vector<WorldPosition> points_of_interest;
public:
    void restart()
    {
        cops_on_the_street.clear();
        on_patrol_mission.clear();
    }

    void update(Bounds const& simulation_bounds,
                CollisionManager& collisions,
                int turn_count)
    {
        auto alert_mode = late_hours(turn_count);

        for(auto const& r: criminal_records) {
            // Decide which district holds the most amount of criminals
        }

        if (turn_count % spawn_interval == 0) {
            srand(static_cast<unsigned int>(time(0) * turn_count));
            const auto w = simulation_bounds.maxx - simulation_bounds.minx;
            const auto h = simulation_bounds.maxy - simulation_bounds.miny;
            // spawn new PO near some point of interest
            // FIXME: don't spawn in player vision range
            const auto x = simulation_bounds.minx + rand() % w;
            const auto y = simulation_bounds.miny + rand() % h;
            const auto pos = Position{ x, y };

            const auto missionTarget = Position { x + 5, y + 5 };
            const auto id = collisions.add_moving_entity(pos);
            const auto mission = PatrolMission{
                static_cast<unsigned int>(id),
                missionTarget
            };
            cops_on_the_street.push_back(id);
            on_patrol_mission.push_back(mission);
        }

//        auto random_velocities = std::vector<Velocity>(cops_on_the_street.size());

        for(auto const i: cops_on_the_street) {
            const auto v = Velocity{
                generate_random_int(-1, 1),
                generate_random_int(-1, 1)
            };
            collisions.change_velocity(i, v);
        }
    }
    
    auto check_crime_history(id_name const& id) const
    {
        return criminal_records.at(id);
    }

    auto record_crimes(std::vector<PoliceAlert> const& alerts, std::vector<std::string> & message_log) {
        for(auto const& a: alerts) {
            auto id = a.id;
            auto level = a.violation_level;
            auto pos = a.pos;
            auto it = criminal_records.find(id);

            message_log.push_back("Registered crime");

            if (it != std::end(criminal_records)) {
                it->second.violation_level += level;
                // FIXME: Pass district id
                it->second.last_known_location = pos;
            }
        }
    }
};


#endif /* police_h */
