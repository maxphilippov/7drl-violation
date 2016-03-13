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
    const static int default_drain;
    const static int max_charge;
    BatteryManager(int charge = max_charge) : current_charge(charge) {}

    auto get_charge() const {
        return static_cast<float>(current_charge * 100.0f) / static_cast<float>(max_charge);
    }

    auto charge(int amount = max_charge)
    {
        current_charge = std::min(current_charge + amount, max_charge);
        return current_charge;
    }

    auto discharge(int amount = default_drain)
    {
        current_charge = std::max(current_charge - amount, 0);
        return current_charge;
    }
};

// FIXME:
const int BatteryManager::max_charge = 500;
const int BatteryManager::default_drain = 1;

#endif /* battery_h */
