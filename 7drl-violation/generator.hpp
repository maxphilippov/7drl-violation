//
//  generator.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef generator_h
#define generator_h

#include <algorithm>
#include <random>
#include <memory>

#include "map.hpp"
#include "position.hpp"

class Building
{
    Bounds bounds;
    std::vector<Position> doors;
};

class CityBlock
{
    std::unique_ptr<CityBlock> blockA, blockB;
    std::vector<Building> buildings;
    Bounds bounds;
public:
    CityBlock(Bounds bounds) :
    blockA(nullptr),
    blockB(nullptr),
    bounds(bounds) {}

    // Go deep till the "leaves" and get its bounds
    std::vector<Bounds> get_bounds()
    {
        auto r = std::vector<Bounds>();

        if (blockA != nullptr && blockB != nullptr) {
            auto boundsA = blockA->get_bounds();
            r.insert(std::end(r), std::begin(boundsA), std::end(boundsA));
            auto boundsB = blockB->get_bounds();
            r.insert(std::end(r), std::begin(boundsB), std::end(boundsB));
        } else {
            r.push_back(bounds);
        }

        return r;
    }

    // Divide block while it's bigger than smallest_block
    void divide(int seed, MapSize const& smallest_block)
    {
        auto generator = std::default_random_engine(seed);
        auto d = std::uniform_int_distribution<>(0,1);

        auto dir = d(generator);

        Bounds boundsA, boundsB;

        if (dir == 0) {
            // split horizontally
            auto middle_point = bounds.minx + (bounds.maxx - bounds.minx) / 2;
            auto d = std::uniform_int_distribution<>((int)middle_point * 0.9, (int)middle_point * 1.1);

            auto slice = d(generator);

            boundsA = {
                bounds.minx, bounds.miny,
                slice, bounds.maxy
            };

            boundsB = {
                slice, bounds.miny,
                bounds.maxx, bounds.maxy
            };

        } else {
            // split vertically
            auto middle_point = bounds.miny + (bounds.maxy - bounds.miny) / 2;
            auto d = std::uniform_int_distribution<>((int)middle_point * 0.9, (int)middle_point * 1.1);

            auto slice = d(generator);

            boundsA = {
                bounds.minx, bounds.miny,
                bounds.maxx, slice
            };

            boundsB = {
                bounds.minx, slice,
                bounds.maxx, bounds.maxy
            };
        }

        // TODO: Fix that, can go crazy infinite, temporary (* 2)
        if (bounds.maxx - bounds.minx > smallest_block.width * 2 &&
            bounds.maxy - bounds.miny > smallest_block.height * 2 ) {
            blockA = std::make_unique<CityBlock>(boundsA);
            blockB = std::make_unique<CityBlock>(boundsB);
            blockA->divide(seed + 1, smallest_block);
            blockB->divide(seed + 2, smallest_block);
        }
    }
};

MapCells& paintBounds(MapCells& cells,
                      Bounds const& mapSize,
                      std::vector<Bounds> const& paintInside,
                      MapTile value,
                      MapTile borderValue
                      )
{
    auto mapWidth = mapSize.maxx - mapSize.minx;

    for(auto const& b: paintInside) {
        auto minx = std::max(mapSize.minx, b.minx);
        auto miny = std::max(mapSize.miny, b.miny);
        auto maxx = std::min(mapSize.maxx, b.maxx);
        auto maxy = std::min(mapSize.maxy, b.maxy);
        for(auto j = miny; j < maxy; ++j) {
            for(auto i = minx; i < maxx; ++i) {
                auto pos = i + j * mapWidth;
                cells.at(pos) = (j != miny && i != minx && j != maxy - 1 && i != maxx - 1) ? value : borderValue;
            }
        }
    }

    return cells;
}

MapCells generate(int seed, MapSize const& size) {
    auto bounds = Bounds{ 0, 0, size.width, size.height };

    auto r = MapCells(size.width * size.height, MapTile::Road);

    auto rootBlock = CityBlock(bounds);

    auto minwidth = std::max(size.width / 12, 6);
    auto minheight = std::max(size.height / 12, 6);
    rootBlock.divide(seed, MapSize{ minwidth, minheight });

    auto all_bounds = rootBlock.get_bounds();

    std::for_each(std::begin(all_bounds),
                  std::end(all_bounds),
                  [](auto& b) { b = b.shrink(3); });

    paintBounds(r, bounds, all_bounds, MapTile::Empty, MapTile::Wall);

    return r;
}

#endif /* generator_h */
