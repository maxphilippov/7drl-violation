//
//  position.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef position_h
#define position_h

#include <algorithm>
#include <vector>

// By some dude on stackoverflow
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

struct Position
{
    int x;
    int y;
};

std::vector<Position> draw_line(Position const& from, Position const& to)
{
    auto dx = from.x - to.x;
    auto dy = from.y - to.y;
    // Length excluding from and to
    auto length = std::max(std::abs(dx), std::abs(dy)) - 1;
    if (length > 0) {

        auto midx = static_cast<float>(std::abs(dx) / 2);
        auto midy = static_cast<float>(std::abs(dy) / 2);

        auto mid_point = Position {
            std::min(from.x, to.x) + static_cast<int>(std::round(midx)),
            std::min(from.y, to.y) + static_cast<int>(std::round(midy))
        };

        auto first_half = draw_line(from, mid_point);
        auto second_half = draw_line(mid_point, to);

        // Bjarne forgive me, I'm so bad at this
        auto line = std::vector<Position>(first_half);
        line.insert(std::end(line), std::begin(second_half), std::end(second_half));

        return line;
    }
    return {to};
}

auto squared_distance(Position const& p1, Position const& p2)
{
    auto dx = p1.x - p2.x;
    auto dy = p1.y - p2.y;

    return dx*dx + dy*dy;
}

struct WorldPosition
{
    unsigned int district_id;
    Position pos;
};

// [minx, maxx) [miny, maxy)
struct Bounds
{
    int minx, miny;
    int maxx, maxy;
    
    auto contains(Position const& pos) const
    {
        return pos.x >= minx && pos.y >= miny && pos.x < maxx && pos.y < maxy;
    }

    auto intersects(Bounds const& o) const
    {
        // r = half_size
        // c = center
        // 1 = this obj, 2 = other object
        auto r_x_1 = (maxx - minx) / 2;
        auto c_x_1 = r_x_1 + minx;
        auto r_y_1 = (maxy - miny) / 2;
        auto c_y_1 = r_x_1 + miny;

        auto r_x_2 = (o.maxx - o.minx) / 2;
        auto c_x_2 = r_x_2 + o.minx;
        auto r_y_2 = (o.maxy - o.miny) / 2;
        auto c_y_2 = r_x_2 + o.miny;

        auto x = std::abs(c_x_1 - c_x_2) <= (r_x_1 + r_x_2);
        auto y = std::abs(c_y_1 - c_y_2) <= (r_y_1 + r_y_2);

        return x && y;
    }
    
    auto shrink(int value) const
    {
        return Bounds {
            minx + value, miny + value,
            maxx - value, maxy - value
        };
    }
};

auto from_center(Position const& center, int width, int height) {
    auto x_first_half = width / 2;
    auto x_second_half = width - x_first_half;
    auto y_first_half = height / 2;
    auto y_second_half = height - y_first_half;

    return Bounds {
        center.x - x_first_half, center.y - y_first_half,
        center.x + x_second_half, center.y + y_second_half
    };
}

struct Velocity
{
    int x;
    int y;
};

// FIXME: not working as I expected
auto towards(Position const& a, Position const& b) {
    return Velocity {
        sgn(b.x - a.x),
        sgn(b.y - a.y)
    };
}

#endif /* position_h */
