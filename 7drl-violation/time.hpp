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
    operator int() { return static_cast<int>(h * turns_in_hour); }
};

auto turns_to_hours(int turns) {
    return Hours{ static_cast<float>(turns / Hours::turns_in_hour) };
}

#endif /* time_h */
