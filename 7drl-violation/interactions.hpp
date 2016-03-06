//
//  interactions.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef interactions_h
#define interactions_h

#include <vector>
#include <unordered_map>

enum InteractionType
{
    Travel,
    JobComplete,
    Dialog
};

struct InteractionData
{
    InteractionType id;
};

struct Interaction
{
    std::function<std::string(InteractionData const&)> func;
};

class InteractionQueue
{
    static const std::unordered_map<InteractionType, Interaction> interaction_pool;
    // We just iterate on this once per turn, so it's a vector
    std::vector<InteractionData> interactions;
public:
    InteractionQueue()
    {
        interactions.reserve(10);
    }
    
    void add(InteractionType id) {
        auto d = InteractionData {
            id
        };
        interactions.push_back(d);
    }
    
    void run(std::vector<std::string>& message_log)
    {
        for(auto i: interactions) {
            auto inter = interaction_pool.at(i.id);
            auto r = inter.func(i);
            message_log.push_back(r);
        }
        
        interactions.clear();
    }
};

const std::unordered_map<InteractionType, Interaction> InteractionQueue::interaction_pool = {
    { InteractionType::Travel, { [](InteractionData const&) { return "Travel"; } } },
    { InteractionType::JobComplete, { [](InteractionData const&) { return "R-r-r-ring. Your order is complete"; } } }
};


#endif /* interactions_h */
