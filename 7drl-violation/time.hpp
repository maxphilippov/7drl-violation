//
//  time.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef time_h
#define time_h

class Hours
{
    float h;
public:
    static const auto turns_in_hour = 12;
    Hours(float h) : h(h) {}
    // I'd like to make it return a Turns type, but
    // has to overload all operators, so it's int for the moment
    auto to_turns() const { return static_cast<int>(h * turns_in_hour); }
    operator float() { return h; }
};

auto turns_to_hours(int turns) {
    return Hours{ static_cast<float>(turns) / Hours::turns_in_hour };
}

auto late_hours(int current_turn) {
    auto h = turns_to_hours(current_turn);

    auto day = h / 24.0f;
    auto hour_in_day = static_cast<int>(h) % 24;

    return 19 < hour_in_day || hour_in_day < 7;
}

#endif /* time_h */
