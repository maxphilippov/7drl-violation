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
#include "message_log.hpp"

#include "city.hpp"
#include "time.hpp"
#include "collisions.hpp"
#include "position.hpp"
#include "interaction_types.hpp"

// PO - Police officer

struct PatrolMission
{
    physical_object_id_type active_po_id;
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
    static const int officer_reaction_range = 7 * 7;
    static const int spawn_interval = 8;
    static const int nap_interval = 7;

    typedef int violation_level;
    // I don't need a fast lookup actually cause getting one value
    // out of this map is performed only when a PO checks your ID
    std::map<identity_id_type, CriminalRecord> criminal_records;

    std::vector<physical_object_id_type> cops_on_the_street;

    std::vector<PatrolMission> on_patrol_mission;

    std::vector<WorldPosition> points_of_interest;

    void spawn(CollisionManager & collisions,
               Bounds const& simulation_bounds)
    {
        auto const& sb = simulation_bounds;
        // spawn new PO near some point of interest
        const auto x = generate_random_int(sb.minx, sb.maxx);
        const auto y = generate_random_int(sb.miny, sb.maxy);
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

public:
    void restart(CollisionManager & collisions, Bounds const& simulation_bounds)
    {
        cops_on_the_street.clear();
        on_patrol_mission.clear();
        for (auto i = 0; i < 20; ++i) {
            spawn(collisions, simulation_bounds);
        }
    }

    void update(Position const& player, // To raycast vision
                Bounds const& simulation_bounds,
                CollisionManager& collisions,
                CityManager const& city,
                int turn_count)
    {
        auto alert_mode = late_hours(turn_count);

        for(auto const& r: criminal_records) {
            // Decide which district holds the most amount of criminals
        }

        if (turn_count % spawn_interval == 0) {
            spawn(collisions, simulation_bounds);
        }

        if (turn_count % nap_interval != 0 || alert_mode) {
            std::remove_if(std::begin(cops_on_the_street),
                           std::end(cops_on_the_street),
                           [&simulation_bounds, &collisions, &player, &city] (auto i) {
                               auto p = collisions.get_position(i);
                               if (!p.first) return true;

                               auto vision = collisions.check_vision(city, i, player, officer_reaction_range);
                               const auto v = vision ? towards(p.second, player) : Velocity{
                                   generate_random_int(-1, 1),
                                   generate_random_int(-1, 1)
                               };

                               // Returns false if entity was disposed
                               // by collision manager
                               return !collisions.change_velocity(i, v);
                           });
        }
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

    auto record_crimes(std::vector<PoliceAlert> const& alerts, std::vector<std::string> & message_log, int turn_counter) {
        for(auto const& a: alerts) {
            auto id = a.id;
            auto level = a.violation_level;
            auto const& pos = a.pos;
            auto it = criminal_records.find(id);
            
            message_log.push_back(format_message_log("DEBUG: Registered crime", turn_counter));

            if (it != std::end(criminal_records)) {
                it->second.violation_level += level;
                it->second.last_known_location = pos;
            } else {
                criminal_records.insert({id, { pos, level }});
            }
        }
    }

    auto officers_inside(Bounds const& bounds)
    {
        return 0;
    }
};


#endif /* police_h */
