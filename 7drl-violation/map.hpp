//
//  map.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef map_h
#define map_h

typedef int MapSide;

struct MapSize
{
    MapSide width;
    MapSide height;
};

typedef std::vector<int> MapCells;

#endif /* map_h */
