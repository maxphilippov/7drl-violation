//
//  map.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef map_h
#define map_h

#include <vector>
#include <unordered_set>

#include "position.hpp"

enum MapTile {
    Empty,
    Wall,
    Road,
    Door,
    Crowd,
    Bar,
    Clinic,
    Station,
    Repairs
};

const std::unordered_set<MapTile> interactive_tiles = {
    MapTile::Bar, MapTile::Clinic, MapTile::Repairs, MapTile::Station
};

typedef int MapSide;

struct MapSize
{
    MapSide width;
    MapSide height;
};

typedef std::vector<MapTile> MapCells;
typedef MapCells::size_type MapIndex;

MapIndex get_map_position(MapCells const& map, MapSize const& size, Position const& pos)
{
    auto x = std::max(0, std::min(pos.x, size.width));
    auto y = std::max(0, std::min(pos.y, size.height));
    return x + y * size.width;
}

#endif /* map_h */
