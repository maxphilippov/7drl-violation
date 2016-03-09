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
    const std::unordered_map<MapTile, char> mapTiles = {
        { MapTile::Empty, '.' }, // Empty space
        { MapTile::Wall, '#' }, // Wall
        { MapTile::Road, '+' }, // Road
        { MapTile::Door, '=' }, // Door
        { MapTile::Crowd, 'C' },
    };

    const CharMap actors = {
        { 0, '@' }, // Player
        { 2, 'P' }, // Police officer
        { 3, 'G' }, // Gangstar
        { 4, 'D' }, // Android
        { 5, 'M' }, // Master
    };
}

#endif /* symbols_h */
