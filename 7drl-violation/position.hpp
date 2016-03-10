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

struct WorldPosition
{
    int district_id;
    Position district_position;
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

// By some dude on stackoverflow
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

auto towards(Position const& a, Position const& b) {
    return Velocity {
        sgn(a.x - b.x),
        sgn(a.y - b.y)
    };
}

#endif /* position_h */
