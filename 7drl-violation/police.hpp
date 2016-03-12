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

#include "basic_types.hpp"

#include "time.hpp"
#include "collisions.hpp"
#include "position.hpp"
#include "interaction_types.hpp"

// PO - Police officer

struct ActivatedPO
{
    physical_object_id_type collision_id;
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
    static const int officer_reaction_range = 7;
    static const int spawn_interval = 8;

    typedef int violation_level;
    // I don't need a fast lookup actually cause getting one value
    // out of this map is performed only when a PO checks your ID
    std::map<identity_id_type, CriminalRecord> criminal_records;

    std::vector<physical_object_id_type> cops_on_the_street;

    std::vector<PatrolMission> on_patrol_mission;

    std::vector<WorldPosition> points_of_interest;
public:
    void restart()
    {
        cops_on_the_street.clear();
        on_patrol_mission.clear();
    }

    void update(Position const& player, // To raycast vision
                Bounds const& simulation_bounds,
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

        std::remove_if(std::begin(cops_on_the_street),
                       std::end(cops_on_the_street),
                       [&collisions] (auto i) {
                           const auto v = Velocity{
                               generate_random_int(-1, 1),
                               generate_random_int(-1, 1)
                           };

                           // Returns false if entity was disposed
                           // by collision manager
                           return !collisions.change_velocity(i, v);
                       });
    }

    auto check_crime_history(identity_id_type id, WorldPosition const& pos)
    {
        auto it = criminal_records.find(id);

        if (it != std::end(criminal_records)) {
            it->second.last_known_location = pos;
            return it->second.violation_level;
        }

        return 0;
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

    auto officers_inside(Bounds const& bounds)
    {
        return 0;
    }
};


#endif /* police_h */
