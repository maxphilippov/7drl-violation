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

class CharManager
{
public:
    const static CharMap mapTiles;
    const static CharMap actors;
};

const CharMap CharManager::mapTiles = {
    { 0, "." }, // Empty space
    { 1, "#" }, // Wall
    { 2, "+" }, // Road
    { 3, "=" }, // Door
};

const CharMap CharManager::actors = {
    { 0, "@" }, // Player
    { 1, "C" }, // Citizen
    { 2, "P" }, // Police officer
};

#endif /* symbols_h */
