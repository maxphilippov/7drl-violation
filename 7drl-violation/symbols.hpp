//
//  symbols.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef symbols_h
#define symbols_h

#include <string>
#include <unordered_map>

typedef std::unordered_map<int, std::string> CharMap;

namespace symbols
{
    const CharMap mapTiles = {
        { 0, "." }, // Empty space
        { 1, "#" }, // Wall
        { 2, "+" }, // Road
        { 3, "=" }, // Door
    };

    const CharMap actors = {
        { 0, "@" }, // Player
        { 1, "C" }, // Citizen
        { 2, "P" }, // Police officer
        { 3, "G" }, // Gangstar
        { 4, "D" }, // Android
    };
}

#endif /* symbols_h */
