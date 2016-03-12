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
#include <map>

#include "map.hpp"
#include "interaction_types.hpp"
#include "position.hpp"
#include "city.hpp"

#include "quadtree.hpp"

std::unordered_set<MapTile> unpassable_tiles = {
    MapTile::Wall
};

std::unordered_set<MapTile> obscuring_tiles = {
    MapTile::Wall, MapTile::Crowd
};

struct PosVel
{
    Position pos;
    Velocity vel;
};

struct ActorCollisionInfo
{
    physical_object_id_type first;
    physical_object_id_type second;
};

class CollisionManager
{
    typedef std::map<physical_object_id_type, PosVel> ObjectContainer;
    ObjectContainer objects;

    physical_object_id_type next_id = 0;
public:
    void restart()
    {
        objects.clear();
    }

    void update(Bounds const& simulation_bounds,
                CityManager const& city,
                std::vector<ActorCollisionInfo>& collisions)
    {
        auto const& level_bounds = city.bounds();
        // Update all entities positions (actually should update just nearest to the player)
        auto new_objects = objects;

        // Dispose objects outside of bounds
        {
            auto begin = std::begin(new_objects);
            auto end_iter = std::end(new_objects);

            for(auto iter = begin; iter != end_iter; ) {
                if (!simulation_bounds.contains(iter->second.pos)) {
                    new_objects.erase(iter++);
                } else {
                    auto const& p = iter->second.pos;
                    auto const& v = iter->second.vel;
                    auto new_pos = Position{
                        std::max(0, std::min(p.x + v.x, level_bounds.width - 1)),
                        std::max(0, std::min(p.y + v.y, level_bounds.height - 1))
                    };


                    auto tile = city.get(new_pos);
                    auto collision = (unpassable_tiles.count(tile) == 0);
                    iter->second.pos = collision ? new_pos : p;
                    iter->second.vel = Velocity{ 0, 0 };
                    ++iter;
                }
            }
        }
        {
            auto begin = std::begin(new_objects);
            auto end_iter = std::end(new_objects);
            for(auto const& kv: new_objects) {
                auto const& new_pos = kv.second.pos;
                auto collision_entity = std::find_if(begin, end_iter,
                                                     [&new_pos](auto const& kv) {
                                                         auto const &p = kv.second.pos;
                                                         return p.x == new_pos.x &&
                                                         p.y == new_pos.y;
                                                     });
                if(collision_entity != end_iter && collision_entity->first != kv.first) {
                    collisions.push_back({
                        collision_entity->first,
                        kv.first
                    });

                    collision_entity->second.pos = objects.find(collision_entity->first)->second.pos;
                }
            }
        }

        objects = new_objects;
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

        if (!bounds.contains(pos)) {
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
    auto change_velocity(physical_object_id_type id, Velocity vel)
    {
        auto it = objects.find(id);

        auto found = it != std::end(objects);

        if (found) {
            it->second.vel = vel;
        }

        return found;
    }

    auto get_position(physical_object_id_type id) const
    {
        auto it = objects.find(id);
        auto r = std::pair<bool, Position>{ false, Position {} };
        if (it != std::end(objects)) {
            r.first = true;
            r.second = it->second.pos;
        }
        return r;
    }

    auto check_cells_vision() const
    {
        
    }

    auto check_vision(CityManager const& city,
                      physical_object_id_type id,
                      Position const& pos,
                      int sqr_max_range) const
    {
        auto const& pair = get_position(id);
        if (pair.first) {
            auto distance = squared_distance(pair.second, pos);

            if (distance > sqr_max_range) {
                return false;
            }
            auto line = draw_line(pair.second, pos);

            for(auto const& p: line) {
                auto tile = city.get(p);

                if(obscuring_tiles.count(tile) !=0 ) return false;
            }
            return true;
        }
        return false;
    }

    auto add_moving_entity(Position pos, Velocity vel = Velocity{0, 0})
    {
        auto id = next_id;

        objects.insert({ next_id, { pos, vel } });

        ++next_id;

        return id;
    }

    auto get_in_range(Position const& player, MapSize const& half_screen) const
    {
        auto bounds = Bounds{
            player.x - half_screen.width,
            player.y - half_screen.height,
            player.x + half_screen.width,
            player.y + half_screen.height
        };

        auto r = std::vector<Position>();

        r.reserve(objects.size());

        for(auto const& kv: objects) {
            auto const& p = kv.second.pos;
            if (bounds.contains(p)) {
                r.push_back(p);
            }
        }
        
        return r;
    }
};

#endif /* collisions_h */
