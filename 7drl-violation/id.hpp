//
//  id.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef id_h
#define id_h

#include <string>
#include <vector>
#include <random>

#include "basic_types.hpp"

struct IDData
{
    identity_id_type id;
    
    std::string name;
    
    enum Type
    {
        human,
        android,
        ITEMS_COUNT
    } type;
    
    int balance;

    auto is_human() const {
        return type == Type::human;
    }

    auto is_android() const {
        return type == Type::android;
    }

    static auto type_to_string(Type t) {
        return (t == Type::human) ? "human" : "android";
    }
};

const auto player_real_identity = IDData{1, "Mikoto", IDData::android, 5000};

namespace
{
    const std::vector<std::string> names_pool = {
        "Aiko", "Aimi", "Airi", "Akira", "Chinatsu", "Chiyo", "Emi", "Haruko"
    };
}

class IDGenerator
{
    // 1 is reserved for player id, bad decision, and we need unresolved crimes
    identity_id_type id = 2;

    auto random_type() const
    {
        std::default_random_engine rd;
        std::mt19937 gen(rd());
        auto max = static_cast<int>(IDData::Type::ITEMS_COUNT);
        std::uniform_int_distribution<> dist{0, max};
        auto val = dist(gen);
        return static_cast<IDData::Type>(val);
    }

    auto random_name() const
    {
        std::default_random_engine rd;
        std::mt19937 gen(rd());

        auto max = names_pool.size();
        std::uniform_int_distribution<unsigned long> dist{0, max - 1};

        auto pos = dist(gen);
        return names_pool.at(pos);
    }

public:
    auto next(int balance)
    {
        auto new_id = IDData {
            id,
            random_name(),
            random_type(),
            balance
        };
        ++id;

        return new_id;
    }
};


// One id is attached to you
// You can have a phone with a fake ID
//
// Every time in a public place when someone ask you for your id
// your phone can hide your original ID and show a fake one
// If you get caught and PO brings you to the station the phone won't help
// you and it's game over
//
// You use your ID to pay for stuff. Every citizen is allowed to borrow money
// (means his balance can go below zero). But your fake IDs aren't good ones
// so for you borrowing money is a law violation
// To check your balance shop will need 1-2 hours, so after that if you had no
// money they'll call police at that place


#endif /* id_h */
