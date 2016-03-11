//
//  battery.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef battery_h
#define battery_h

class BatteryManager
{
    int current_charge;

public:
    const static auto max_charge = 100;
    BatteryManager(int charge) : current_charge(charge) {}

    auto get_charge() const { return current_charge; }
    auto do_charge(int amount)
    {
        charge = std::min(charge + amount, max_charge);
        return current_charge;
    }
};

#endif /* battery_h */
