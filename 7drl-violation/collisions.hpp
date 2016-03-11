//
//  collisions.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef collisions_h
#define collisions_h

#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>

#include "map.hpp"
#include "interaction_types.hpp"
#include "position.hpp"
#include "city.hpp"

std::unordered_set<MapTile> unpassable_tiles = {
    MapTile::Wall
};

struct PhysicalData
{
    typedef unsigned int id_type;
    std::vector<Position> positions;
    std::vector<Velocity> velocities;
    std::vector<id_type> ids;

    id_type next_id;

    auto add_object(Position pos, Velocity vel)
    {
        auto id = next_id;

        ids.push_back(id);
        positions.push_back(pos);
        velocities.push_back(vel);

        next_id += 1;

        return id;
    }

    auto get_position(id_type id) const
    {
        auto it = std::find(std::cbegin(ids),
                            std::cend(ids),
                            id);

        auto r = std::pair<bool, Position>{ false, Position{} };
        if (it != std::cend(ids)) {
            auto idx = it - std::cbegin(ids);
            auto pos_it = std::cbegin(positions) + idx;
            r.first = true;
            r.second = *pos_it;

        }
        return r;
    }

    auto change_velocity(id_type id, Velocity vel)
    {
        auto it = std::find(std::cbegin(ids),
                            std::cend(ids),
                            id);

        if (it != std::cend(ids)) {
            auto idx = it - std::cbegin(ids);

            velocities.at(idx) = vel;
        }

        return it != std::cend(ids);
    }

    void remove_object(id_type id)
    {
        auto it = std::find(std::cbegin(ids),
                            std::cend(ids),
                            id);

        if(it != std::cend(ids)) {
            auto idx = it - std::cbegin(ids);
            positions.erase(std::begin(positions) + idx);
            velocities.erase(std::begin(velocities) + idx);
            ids.erase(it);
        }
    }
};

class CollisionManager
{
    PhysicalData all_data;
public:

    CollisionManager()
    {
    }

    void update(Bounds const& simulation_bounds,
                CityManager const& city,
                std::vector<DialogData>& dialogs)
    {
        auto& velocities = all_data.velocities;
        auto& positions = all_data.positions;

        auto level_bounds = city.bounds();
        // Update all entities positions (actually should update just nearest to the player)
        auto new_positions = std::vector<Position>(positions.size());

        // To reduce allocations in next transform lambdas
        auto b_positions = std::cbegin(positions);
        auto e_positions = std::cend(positions);

        std::transform(b_positions,
                       e_positions,
                       std::begin(velocities),
                       std::begin(new_positions),
                       [this, &level_bounds, &city, &b_positions, &e_positions](auto const& p, auto const& v) {
                           auto const& new_pos = Position{
                               std::max(0, std::min(p.x + v.x, level_bounds.width - 1)),
                               std::max(0, std::min(p.y + v.y, level_bounds.height - 1))
                           };

                           auto tile = city.get(new_pos);

                           auto it = std::find_if(b_positions,
                                                  e_positions,
                                                  [&new_pos](auto const& p) {
                                                      return new_pos.x == p.x &&
                                                      new_pos.y == p.y;
                                                  });

                           if (it != e_positions) {
                               return p;
                           }

                           return (unpassable_tiles.count(tile) == 0) ? new_pos : p;
                       });

        // TODO: Actually check collisions
        // For collision checking we might need to sort a vector,
        // that makes the whole id thing look bad
        // Looks like I gotta change how data is stored,
        // Cause we'll need to check object priorities when two entities try to enter same cell
        // Police officer > Player > Others

        std::swap(positions, new_positions);

        // TODO: Find a better way to nullify every velocity
        std::transform(std::begin(velocities),
                       std::end(velocities),
                       std::begin(velocities),
                       [](auto const& v) {
                           return Velocity{ 0, 0 };
                       });

        // TODO: On every collision generate an interaction
    }

    bool raycast(MapCells const& cells, Position const& from, Position const& to) const
    {
        // Draw simple line and check if there are no walls or crowds on the way
        return true;
    }

    auto check_free_cells(CityManager const& city, std::vector<Position> const& poss) const
    {
        auto free_pos = std::vector<Position>(poss.size());
        auto it = std::copy_if(std::begin(poss),
                               std::end(poss),
                               std::begin(free_pos),
                               [&city](auto const& p) {
                                   auto tile = city.get(p);
                                   return (unpassable_tiles.count(tile) == 0);
                               } );
        free_pos.resize(std::distance(std::begin(free_pos), it));

        return free_pos;
    }

    auto nearest_free_cell(CityManager const& city, Position const& pos) const
    {
        auto city_size = city.bounds();

        auto bounds = Bounds {
            0, 0, city_size.width, city_size.height
        };

        if (!bounds.inside(pos)) {
            // FIXME: Optional value
            return Position { -1, -1 };
        }

        auto tile = city.get(pos);

        if (unpassable_tiles.count(tile) == 0) {
            return pos;
        } else {
            // FIXME: iterate over neighbours and increase range if there's no
            // free cells around
            return nearest_free_cell(city, Position{ pos.x - 1, pos.y - 1 });
        }
    }

    // Return false if there's no such object registered for collisions
    // which means we disposed it already
    bool change_velocity(PhysicalData::id_type id, Velocity vel)
    {
        return all_data.change_velocity(id, vel);
    }

    auto get_position(PhysicalData::id_type id)
    {
        return all_data.get_position(id);
    }

    void teleport(PhysicalData::id_type id, Position pos)
    {
//        positions.at(id) = pos;
    }

    auto add_moving_entity(Position pos, Velocity vel = Velocity{0, 0})
    {
        auto id = all_data.add_object(pos, vel);

        return id;
    }

    auto get_in_range(Position const& player, MapSize const& half_screen) const
    {
        auto& positions = all_data.positions;

        auto bounds = Bounds{
            player.x - half_screen.width,
            player.y - half_screen.height,
            player.x + half_screen.width,
            player.y + half_screen.height
        };

        auto r = std::vector<Position>(positions.size());

        auto it = std::copy_if(std::begin(positions),
                               std::end(positions),
                               std::begin(r),
                               [&bounds](auto const& p) { return bounds.inside(p); } );
        
        r.resize(std::distance(std::begin(r), it));
        
        return r;
    }
};

#endif /* collisions_h */
