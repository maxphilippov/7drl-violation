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
        
        if (blockA != nullptr || blockB != nullptr) {
            if (blockA != nullptr) {
                auto boundsA = blockA->get_bounds();
                r.insert(std::end(r), std::begin(boundsA), std::end(boundsA));
            }
            if (blockB != nullptr) {
                auto boundsB = blockB->get_bounds();
                r.insert(std::end(r), std::begin(boundsB), std::end(boundsB));
            }
        } else {
            r.push_back(bounds);
        }
        
        return r;
    }
    
    // Divide block while it's bigger than smallest_block
    void divide(int seed, MapSize const& smallest_block)
    {
        if (bounds.maxx - bounds.minx <= smallest_block.width ||
            bounds.maxy - bounds.miny <= smallest_block.height ) {
            return;
        }
        auto generator = std::default_random_engine(std::time(0));
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
            auto d = std::uniform_int_distribution<int>((int)middle_point * 0.9, (int)middle_point * 1.1);
            
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
        blockA = std::make_unique<CityBlock>(boundsA);
        blockB = std::make_unique<CityBlock>(boundsB);
            blockA->divide(seed, smallest_block);
            blockB->divide(seed, smallest_block);
    }
};

MapCells& paintBounds(MapCells& cells,
                      Bounds const& mapSize,
                      std::vector<Bounds> const& paintInside,
                      int value
                      )
{
    auto mapWidth = mapSize.maxx - mapSize.minx;
    
    for(auto b: paintInside) {
        auto maxx = std::min(mapSize.maxx, b.maxx);
        auto maxy = std::min(mapSize.maxy, b.maxy);
        for(auto j = b.miny; j < maxy; ++j) {
            for(auto i = b.minx; i < maxx; ++i) {
                auto pos = i + j * mapWidth;
                cells.at(pos) = value;
            }
        }
    }
    
    return cells;
}

MapCells generate(int seed, MapSize const& size, int cols, int rows) {
    auto bounds = Bounds{ 0, 0, size.width, size.height };
    
    auto r = MapCells(size.width * size.height);
    
    auto rootBlock = CityBlock{bounds};
    
    rootBlock.divide(seed, MapSize{ size.width / 6, size.height / 6 });
    
    auto all_bounds = rootBlock.get_bounds();
    
    std::for_each(std::begin(all_bounds), std::end(all_bounds), [](Bounds& b) { b = b.shrink(1); });
    paintBounds(r, bounds, all_bounds, 1);
    
    return r;
}

#endif /* generator_h */
