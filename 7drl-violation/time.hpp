//
//  time.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef time_h
#define time_h

#include <cstdlib>

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

auto turns_to_days_hours(int turns) {
    auto r = std::div(turns, Hours::turns_in_hour * 24);

    return std::pair<int, int>{r.quot, r.rem};
}

auto late_hours(int current_turn) {
    auto h = turns_to_hours(current_turn);
    
    auto hour_in_day = static_cast<int>(h) % 24;

    return 19 < hour_in_day || hour_in_day < 7;
}


const Hours purchase_check_default_time = Hours { 1.5f };
const Hours id_check_default_time = Hours { 12.0f };

#endif /* time_h */
