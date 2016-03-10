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
    MapSize size;
    std::vector<int> crowds_map;
public:
    CrowdManager(MapSize size) :
    size(size),
    crowds_map(size.width * size.height) {}
    
    void resize(MapSize newsize)
    {
        size = newsize;
        crowds_map.resize(size.width * size.height);
        std::for_each(std::begin(crowds_map), std::end(crowds_map), [](auto v) { v = 0 });
    }
    
    void spawn_crowd(MapCells const& cells, Position const& pos) {
        auto idx = get_map_position(cells, size, pos);
    }
    
    auto const& data() const
    {
        return crowds_map;
    }
};

// Crowd is like shadow in stealth games
// When player enters a crowd a PO loses sight of her
// Cause crowd blocks sight raycasting like walls do

#endif /* crowds_h */
