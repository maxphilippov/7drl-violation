//
//  city.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef city_h
#define city_h

#include <array>
#include <algorithm>
#include <chrono>
#include <iterator>
#include <vector>

#include "map.hpp"
#include "position.hpp"

#include "generator.hpp"

struct DistrictData
{
    int id;
    
    int population;
    int crime_level;
    int law_power;
    
    int points_of_interest_count;
    
    int private_satelites_count;
};

class District
{
    DistrictData data;
    
    std::array<int, 3> places_count;
    
    std::vector<Position> shops;
    std::vector<Position> clinics;
    std::vector<Position> bars;
public:
    // TODO: Pass data as parameter?
    District(int id, int pop, int crime_level, int law_power, int points_of_interest, int private_satelites) : data{id, pop, crime_level, law_power, points_of_interest, private_satelites}
    {
        std::random_device rd;
        auto gen = std::mt19937(rd());
        // Chance to generate shop / clinic / bar
        auto d = std::discrete_distribution<>({35, 10, 15});
        
        // Now how do we turn that into separate arrays?
        for(auto i = 0; i < points_of_interest; ++i) {
            ++places_count[d(gen)];
        }
    }
    
    DistrictData const& get_data() const
    {
        return data;
    }
};

class CityManager
{
    int district_count;
    
    std::vector<District> used_districts;
    
    MapSize size;
    
    // Current district map
    MapCells district_map;
    
    void add_district()
    {
        auto distr = District{
            static_cast<int>(used_districts.size()),
            0,
            20,
            50,
            6,
            12
        };
        used_districts.push_back(distr);
        district_count -= 1;
    }
    
    void thrash_district(int id)
    {
        auto d = std::find_if(std::begin(used_districts),
                              std::end(used_districts),
                              [id](District const& d)->bool { return d.get_data().id == id; });
        if (d != std::end(used_districts)) {
            used_districts.erase(d);
            district_count += 1;
        }
    }
    
public:
    CityManager() :
    district_count(5),
    size{ 64, 64 },
    district_map(generate(static_cast<unsigned>(std::time(0)), size, 5, 3))
    {
        used_districts.reserve(district_count);
    }
    
    const MapSize& bounds() const
    {
        return size;
    }
    
    const MapCells& map() const
    {
        return district_map;
    }
    
    const auto get(Position const& pos) const
    {
        auto idx = get_map_position(district_map, size, pos);
        return district_map[idx];
    }
    
    const int get(int x, int y) const
    {
        return get(Position{ x, y });
    }
    
    void update(Position const& player)
    {
        
    }
};

#endif /* city_h */
