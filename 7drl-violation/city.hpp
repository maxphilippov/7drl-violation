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


#include "district.hpp"
#include "map.hpp"
#include "position.hpp"

#include "generator.hpp"

class CityManager
{
    int district_count;

    std::vector<District> used_districts;

    std::unordered_multimap<int, int> district_paths;

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
                              [id](auto const& d)->bool { return d.get_data().id == id; });
        if (d != std::end(used_districts)) {
            used_districts.erase(d);
            district_count += 1;
        }
    }

    void generate_districts()
    {
    }
public:
    CityManager(MapSize size) :
    district_count(5),
    size{ size },
    district_map(generate(std::chrono::system_clock::now().time_since_epoch().count(), size))
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

    Position change_district(int id)
    {
        district_map = generate(std::chrono::system_clock::now().time_since_epoch().count(), size);
        const auto player = Position {};

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
