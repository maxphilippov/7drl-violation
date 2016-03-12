//
//  district.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef district_h
#define district_h

#include <array>
#include <random>
#include <vector>

#include "position.hpp"

enum BuildingType
{
    house,
    clinic,
    bar,
    shop, // ? What do we need shop for?
    tube_station,
    BUILDING_TYPES_COUNT
};

struct Building
{
    BuildingType type;
    Bounds bounds;
};

struct DistrictData
{
    int seed;

    int population;
    int crime_level;
    int law_power;

    int points_of_interest_count;

    int private_satelites_count;
};

#endif /* district_h */
