//
//  random.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef random_h
#define random_h

#include <random>

auto generate_random_int(int from, int to, int seed = 0)
{
    std::random_device rdev{};
    std::mt19937 rd{rdev()};
    auto dist = std::uniform_int_distribution<int>{from, to};

    auto result = dist(rd);
    return result;
}


#endif /* random_h */
