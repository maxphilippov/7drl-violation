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
    const static int max_charge;
    BatteryManager(int charge = max_charge) : current_charge(charge) {}

    auto get_charge() const { return current_charge / max_charge * 100; }

    auto charge(int amount)
    {
        current_charge = std::min(current_charge + amount, max_charge);
        return current_charge;
    }

    auto discharge(int amount)
    {
        current_charge = std::max(current_charge - amount, 0);
        return current_charge;
    }
};

const int BatteryManager::max_charge = 300;

#endif /* battery_h */
