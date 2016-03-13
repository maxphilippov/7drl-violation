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

#include "basic_types.hpp"
#include "district.hpp"
#include "map.hpp"
#include "position.hpp"

#include "generator.hpp"

class CityManager
{
    district_id_type next_district_id = 0;

    unsigned int district_count;

    std::unordered_map<district_id_type, DistrictData> used_districts;

    std::unordered_multimap<district_id_type, district_id_type> district_paths;

    MapSize size;

    // Current district map
    MapCells district_map;

    district_id_type current_district = -1;

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
    CityManager(MapSize size, int districts) :
    district_count(districts),
    size{ size }
    {
        used_districts.reserve(district_count);
        district_paths = {
            { 0, 1 },
            { 0, 2 },
            { 0, 3 },
            { 1, 2 },
            { 1, 3 },
            { 1, 0 },
            { 2, 1 },
            { 2, 0 },
            { 3, 4 },
            { 4, 1 },
            { 4, 2 }
        };
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
        return district_map.at(idx);
    }

    const auto get(int x, int y) const
    {
        return get(Position{ x, y });
    }

    auto get_current_district_id() const
    {
        return current_district;
    }

    auto get_neighbour_districts(district_id_type id)
    {
        auto n = std::vector<district_id_type>(district_paths.size());

        auto paths = district_paths.equal_range(id);
        
        auto it = std::transform(paths.first,
                                 paths.second,
                                 std::begin(n),
                                 [&n](auto const& p) {
                                     return p.second;
                                 });

        n.resize(std::distance(std::begin(n), it));

        return n;
    }

    auto change_district(WorldPosition const& location, district_id_type id)
    {
        if (current_district == id) {
            return location;
        }
        auto it = used_districts.find(id);

        std::random_device rd{};
        auto seed = rd() + id;
        if (it != std::end(used_districts))
        {
            seed = it->second.seed;
        }
        else
        {
            add_district(seed);
        }

        district_map = generate(seed, size);
        const auto player = WorldPosition { id, { 2, 2 } };


        current_district = id;
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
