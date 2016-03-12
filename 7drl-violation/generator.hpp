//
//  generator.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef generator_h
#define generator_h

#include <algorithm>
#include <iterator>
#include <random>
#include <memory>

#include "map.hpp"
#include "position.hpp"

class CityBlock
{
    std::unique_ptr<CityBlock> blockA, blockB;
    Bounds bounds;
public:
    CityBlock(Bounds bounds) :
    blockA(nullptr),
    blockB(nullptr),
    bounds(bounds) {}

    // Go deep till the "leaves" and get its bounds
    std::vector<Bounds> all_bounds()
    {
        auto r = std::vector<Bounds>();

        if (blockA != nullptr && blockB != nullptr) {
            auto boundsA = blockA->all_bounds();
            r.insert(std::end(r), std::begin(boundsA), std::end(boundsA));
            auto boundsB = blockB->all_bounds();
            r.insert(std::end(r), std::begin(boundsB), std::end(boundsB));
        } else {
            r.push_back(bounds);
        }

        return r;
    }

    Bounds get_bounds()
    {
        return bounds;
    }

    // Divide block while it's bigger than smallest_block
    void divide(std::mt19937 & generator, MapSize const& smallest_block, int margin = 0)
    {
        if (bounds.maxx - bounds.minx <= smallest_block.width ||
            bounds.maxy - bounds.miny <= smallest_block.height) {
            return;
        }

        auto width = bounds.maxx - bounds.minx;
        auto height = bounds.maxy - bounds.miny;
        auto d = std::discrete_distribution<>({
            static_cast<double>(width / height),
            static_cast<double>(height / width)
        });

        auto dir = d(generator);

        Bounds boundsA, boundsB;

        if (dir == 0) {
            // split horizontally
            auto middle_point = bounds.minx + (bounds.maxx - bounds.minx) / 2;
            auto lower = static_cast<int>(middle_point * 0.8);
            auto higher = static_cast<int>(middle_point * 1.2);
            auto d = std::uniform_int_distribution<>(std::min(lower, higher),
                                                     std::max(lower, higher));

            auto slice = d(generator);

            boundsA = {
                bounds.minx, bounds.miny,
                std::min(slice, bounds.maxx), bounds.maxy
            };

            boundsB = {
                std::max(slice + margin, bounds.minx), bounds.miny,
                bounds.maxx, bounds.maxy
            };

        } else {
            // split vertically
            auto middle_point = bounds.miny + (bounds.maxy - bounds.miny) / 2;
            auto lower = static_cast<int>(middle_point * 0.8);
            auto higher = static_cast<int>(middle_point * 1.2);
            auto d = std::uniform_int_distribution<>(std::min(lower, higher),
                                                     std::max(lower, higher));

            auto slice = d(generator);

            boundsA = {
                bounds.minx, bounds.miny,
                bounds.maxx, std::min(slice, bounds.maxy)
            };

            boundsB = {
                bounds.minx, std::max(slice + margin, bounds.miny),
                bounds.maxx, bounds.maxy
            };
        }

        blockA = std::make_unique<CityBlock>(boundsA);
        blockB = std::make_unique<CityBlock>(boundsB);
        blockA->divide(generator, smallest_block, margin);
        blockB->divide(generator, smallest_block, margin);
    }
};

template <typename T>
auto& fill_bounds(std::vector<T>& cells,
                  Bounds const& mapSize,
                  std::vector<Bounds> const& paintInside,
                  T value,
                  T borderValue
                  )
{
    auto mapWidth = mapSize.maxx - mapSize.minx;

    for(auto const& b: paintInside) {
        auto minx = std::max(mapSize.minx, b.minx);
        auto miny = std::max(mapSize.miny, b.miny);
        auto maxx = std::min(mapSize.maxx, b.maxx);
        auto maxy = std::min(mapSize.maxy, b.maxy);
        auto door_pos = Position { minx + 1, miny };
        for(auto j = miny; j < maxy; ++j) {
            for(auto i = minx; i < maxx; ++i) {
                auto pos = i + j * mapWidth;
                cells.at(pos) = (j != miny && i != minx &&
                                 j != maxy - 1 && i != maxx - 1) ||
                                (j == door_pos.y && i == door_pos.x)
                ? value : borderValue;
            }
        }
    }

    return cells;
}

// FIXME: Ok we don't need that
struct MapData
{
    MapCells cells;
    std::vector<BuildingType> buildings_areas;
};

auto building_type_generator(std::mt19937 & gen)
{
    auto lower = static_cast<int>(MapTile::Bar);
    auto higher = static_cast<int>(MapTile::Repairs);

//    std::discrete_distribution<> d{30, 30, 30};
    std::uniform_int_distribution<> d{lower, higher};
    auto val = d(gen);
    return static_cast<MapTile>(val);
}

auto generate(int seed, MapSize const& size) {
    auto level_bounds = Bounds{ 0, 0, size.width, size.height };

    auto r = MapCells(size.width * size.height, MapTile::Road);

    auto rootBlock = CityBlock{ level_bounds };

    auto minwidth = std::max(size.width / 6, 6);
    auto minheight = std::max(size.height / 6, 6);

    std::mt19937 gen(seed);

    rootBlock.divide(gen, MapSize{ minwidth, minheight });

    auto all_bounds = rootBlock.all_bounds();

    std::for_each(std::begin(all_bounds),
                  std::end(all_bounds),
                  [](auto& b) { b = b.shrink(4); }
                  );

    fill_bounds(r, level_bounds, all_bounds, MapTile::Empty, MapTile::Empty);

    auto building_bounds = std::vector<Bounds>();

    std::for_each(std::begin(all_bounds),
                  std::end(all_bounds),
                  [&r, &gen, &building_bounds](auto const& b) {
                      auto block = CityBlock{ b };
                      block.divide(gen, MapSize{ 8, 8 }, 2);
                      auto bounds = block.all_bounds();
                      building_bounds.insert(std::end(building_bounds),
                                             std::begin(bounds),
                                             std::end(bounds));
                  });

    auto random_tiles = std::vector<MapTile>(building_bounds.size());

    std::generate(std::begin(random_tiles),
                  std::end(random_tiles),
                  std::bind(building_type_generator, gen));

    std::random_shuffle(std::begin(building_bounds), std::end(building_bounds));

    {
        auto iter = std::begin(random_tiles);
        auto end_iter = std::cend(random_tiles);
        auto last_symbol = MapTile::Empty;
        auto b_iter = std::begin(building_bounds);
        auto last_b_begin = b_iter;
        for(; iter != end_iter; ++iter, ++b_iter ) {
            if (last_symbol != *iter || iter + 1 == end_iter) {
                auto v = std::vector<Bounds>(last_b_begin, b_iter);
                fill_bounds(r, level_bounds, v, last_symbol, MapTile::Wall);
                last_b_begin = b_iter;
                last_symbol = *iter;
            }
        }

    }
    auto data = MapData
    {
        r
    };

    return r;
}

#endif /* generator_h */
