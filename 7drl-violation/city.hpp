//
//  city.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef city_h
#define city_h

#include <algorithm>
#include <chrono>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <random>

#include "district.hpp"
#include "map.hpp"
#include "position.hpp"

#include "generator.hpp"

class CityManager
{
    typedef unsigned int district_id_type;
    district_id_type next_district_id = 0;

    unsigned int district_count;

    std::unordered_map<district_id_type, DistrictData> used_districts;

    std::unordered_multimap<int, int> district_paths;

    MapSize size;

    // Current district map
    MapCells district_map;

    auto add_district(int seed)
    {
        auto id = next_district_id;
        auto distr = DistrictData{
            seed
        };
        used_districts.insert({id, distr});
        district_count -= 1;
        ++next_district_id;
    }
public:
    CityManager(MapSize size) :
    district_count(5),
    size{ size }
    {
        used_districts.reserve(district_count);
    }

    const auto& bounds() const
    {
        return size;
    }

    const auto& map() const
    {
        return district_map;
    }

    const auto get(Position const& pos) const
    {
        auto idx = get_map_position(district_map, size, pos);
        return district_map[idx];
    }

    const auto get(int x, int y) const
    {
        return get(Position{ x, y });
    }

    const auto get_neighbour_districts(int id)
    {
        auto n = std::vector<int>();
    }

    auto change_district(district_id_type id)
    {
        auto it = used_districts.find(id);

        std::random_device rd{};
        auto seed = rd() * id;
        if (it != std::end(used_districts))
        {
            seed = it->second.seed;
        }
        else
        {
            add_district(seed);
        }

        district_map = generate(seed, size);
        const auto player = WorldPosition { id, { 0, 0 } };

        return player;
    }

    void update(Bounds const& simulation_bound, MapCells const& crowds_map, int turn_counter)
    {
        std::transform(std::begin(district_map),
                       std::end(district_map),
                       std::begin(crowds_map),
                       std::begin(district_map),
                       [](auto c, auto v) {
                           return v != MapTile::Empty ? v : c;
                       });
    }
};

#endif /* city_h */
