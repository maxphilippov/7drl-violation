//
//  position.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef position_h
#define position_h

struct Position
{
    int x;
    int y;
};

// [minx, maxx) [miny, maxy)
struct Bounds
{
    int minx, miny;
    int maxx, maxy;
    
    bool inside(Position const& pos) const
    {
        return pos.x >= minx && pos.y >= miny && pos.x < maxx && pos.y < maxy;
    }
    
    Bounds shrink(int value) const
    {
        return Bounds {
            minx + value, miny + value,
            maxx - value, maxy - value
        };
    }
};

struct Velocity
{
    int x;
    int y;
};

#endif /* position_h */
