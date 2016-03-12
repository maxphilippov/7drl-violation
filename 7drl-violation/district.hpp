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
    int id;

    int seed;

    int population;
    int crime_level;
    int law_power;

    int points_of_interest_count;

    int private_satelites_count;
};

class District
{
    DistrictData data;

    std::array<unsigned int, 3> places_count;

    std::vector<Position> shops;
    std::vector<Position> clinics;
    std::vector<Position> bars;

    Position entrance_point;
public:
    // TODO: Pass data as parameter?
    District(int id, int pop, int crime_level, int law_power, int points_of_interest, int private_satelites) :
    data{id, pop, crime_level, law_power, points_of_interest, private_satelites}
    {
        std::random_device rd;
        auto gen = std::mt19937{rd()};
        // Chance to generate shop / clinic / bar
        auto d = std::discrete_distribution<>({35, 10, 15});

        // Now how do we turn that into separate arrays?
        for(auto i = 0; i < points_of_interest; ++i) {
            ++places_count[d(gen)];
        }
    }

    auto const& entrance() const
    {
        return entrance_point;
    }

    DistrictData const& get_data() const
    {
        return data;
    }
};

#endif /* district_h */
