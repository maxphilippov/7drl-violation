//
//  crowds.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef crowds_h
#define crowds_h

#include <algorithm>
#include <iterator>
#include <vector>

#include "random.hpp"

#include "city.hpp"
#include "map.hpp"
#include "position.hpp"

class CrowdManager
{
    static const auto spawn_interval = 3;
    MapSize size;
    MapCells crowds_map;

    std::vector<Position> crowd_centers;

    std::unordered_set<unsigned long> spawned_crowds;

    unsigned long crowds_limit = 80;

    auto spawn_crowd(Bounds const& inside, MapCells const& map, Position const& pos)
    {
        auto idx = get_map_position(map, size, pos);

        crowds_map.at(idx) = MapTile::Crowd;

        for(auto i = 0; i < 12; ++i) {
            auto x = generate_random_int(-2, 2);
            auto y = generate_random_int(-2, 2);
            auto idx = get_map_position(map, size, Position{ pos.x + x, pos.y + y });

            auto &tile = crowds_map.at(idx);
            if (tile != MapTile::Crowd) {
                crowds_map.at(idx) = MapTile::Crowd;
            }
        }
        spawned_crowds.insert(idx);
    }
public:
    CrowdManager(MapSize size) :
    size(size),
    crowds_map(size.width * size.height) {}
    
    auto resize(MapSize const& newsize)
    {
        size = newsize;
        crowd_centers.clear();
        crowds_map.clear();
        crowds_map.resize(size.width * size.height);
        std::for_each(std::begin(crowds_map),
                      std::end(crowds_map),
                      [](auto& v) { v = MapTile::Empty; });
    }

    auto update(Bounds const& simulation_bounds, MapCells const& map, int turn_count)
    {
        if (turn_count % spawn_interval == 0 && crowd_centers.size() < crowds_limit) {
            auto center = Position{
                generate_random_int(simulation_bounds.minx, simulation_bounds.maxx),
                generate_random_int(simulation_bounds.miny, simulation_bounds.maxy)
            };

            crowd_centers.push_back(center);

            for(auto const& c: crowd_centers) {
                auto idx = get_map_position(map, size, c);
                if (simulation_bounds.contains(c) && spawned_crowds.count(idx) == 0) {
                    spawn_crowd(simulation_bounds, map, c);
                }
            }
        }
    }
    
    auto const& map() const
    {
        return crowds_map;
    }
};

// Crowd is like shadow in stealth games
// When player enters a crowd a PO loses sight of her
// Cause crowd blocks sight raycasting like walls do

#endif /* crowds_h */
