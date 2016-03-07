//
//  generator.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef generator_h
#define generator_h

#include <random>

#include "map.hpp"
#include "position.hpp"

Bounds divide(int width, int height) {
    return Bounds{
        
    };
}

MapCells& paintBounds(MapCells& cells, Bounds const& mapSize, Bounds const& bounds, int value)
{
    auto mapWidth = mapSize.maxx - mapSize.minx;
    
    auto maxx = std::min(mapSize.maxx, bounds.maxx);
    auto maxy = std::min(mapSize.maxy, bounds.maxy);
    
    for(auto j = bounds.miny; j < maxy; ++j) {
        for(auto i = bounds.minx; i < maxx; ++i) {
            auto pos = i + j * mapWidth;
            cells.at(pos) = value;
        }
    }
    
    return cells;
}

MapCells generate(int seed, MapSize const& size, int cols, int rows) {
    auto generator = std::default_random_engine(seed);
    
    auto bounds = Bounds{ 0, 0, size.width, size.height };
    
    const auto road_half_width = 2;
    
    const auto road_interval = 5 + road_half_width;
    
    auto r = MapCells(size.width * size.height);

    {
        auto randomx = 0;
        auto lastx = 0;
        for(auto i = 0; i < cols; ++i) {
            auto d = std::uniform_int_distribution<int>(lastx, size.width / (cols - i));
            randomx = d(generator);
            auto road = Bounds{
                std::max(randomx - road_half_width, 0),
                0,
                std::min(randomx + road_half_width, size.width),
                size.height
            };
            lastx = randomx + road_interval + 1;
            
            paintBounds(r, bounds, road, 2);
        }
    }
    
    {
        auto randomy = 0;
        auto lasty = 0;
        
        for(auto i = 0; i< rows; ++i) {
            auto d = std::uniform_int_distribution<int>(lasty, size.height / (rows - i));
            randomy = d(generator);
            auto road = Bounds{
                0,
                std::max(randomy - road_half_width, 0),
                size.width,
                std::min(randomy + road_half_width, size.height)
            };
            lasty = randomy + road_interval + 1;
            
            paintBounds(r, bounds, road, 2);
        }
    }
    
    return r;
}

#endif /* generator_h */
