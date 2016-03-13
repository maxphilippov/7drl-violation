//
//  symbols.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef symbols_h
#define symbols_h

#include <unordered_map>

#include "map.hpp"

typedef std::unordered_map<int, char> CharMap;

namespace symbols
{
    const std::unordered_map<MapTile, char> map_tiles = {
        { MapTile::Empty, '.' }, // Empty space
        { MapTile::Wall, '#' }, // Wall
        { MapTile::Road, '=' }, // Road
        { MapTile::Door, ' ' }, // Door
        { MapTile::Crowd, 'C' },
        { MapTile::Bar, 'b' },
        { MapTile::Clinic, 'c' },
        { MapTile::Station, 't' },
        { MapTile::Repairs, 'r' }
    };

    const CharMap actors = {
        { 0, '@' }, // Player
        { 1, 'P' }, // Police officer
        { 2, 'G' }, // Gangstar
        { 3, 'D' }, // Android
        { 4, 'M' }, // Master
    };
}

#endif /* symbols_h */
