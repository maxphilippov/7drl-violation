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

#include "city.hpp"
#include "map.hpp"
#include "position.hpp"

class CrowdManager
{
    static const auto spawn_interval = 3;
    MapSize size;
    MapCells crowds_map;

    std::vector<Position> crowd_centers;
public:
    CrowdManager(MapSize size) :
    size(size),
    crowds_map(size.width * size.height) {}
    
    void resize(MapSize newsize)
    {
        size = newsize;
        crowds_map.resize(size.width * size.height);
        std::for_each(std::begin(crowds_map),
                      std::end(crowds_map),
                      [](auto& v) { v = MapTile::Empty; });
    }

    void update(Bounds const& simulation_bounds, MapCells const& map, int turn_count)
    {
        if (turn_count % spawn_interval == 0) {
            // FIXME:
            srand(turn_count * simulation_bounds.maxx);
            auto center = Position{ rand() % size.width, rand() % size.height };
            spawn_crowd(map, center);
        }
    }
    
    auto const& map() const
    {
        return crowds_map;
    }
private:
    void spawn_crowd(MapCells const& cells, Position const& pos)
    {
        auto idx = get_map_position(cells, size, pos);

        crowds_map.at(idx) = MapTile::Crowd;

        crowd_centers.push_back(pos);
    }
};

// Crowd is like shadow in stealth games
// When player enters a crowd a PO loses sight of her
// Cause crowd blocks sight raycasting like walls do

#endif /* crowds_h */
