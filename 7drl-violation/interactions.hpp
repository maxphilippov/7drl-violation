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

struct TravelData
{
    int destination_id;
};

struct DialogData
{
    
};

void police_officer_interaction()
{
    printw("Sorry, madam, can I check your id");
}

class InteractionQueue
{
    std::vector<TravelData> travels;
    std::vector<DialogData> dialogs;
public:
    InteractionQueue()
    {
        travels.reserve(5);
        dialogs.reserve(5);
    }
    
    void add_travel(int destination_id) {
        travels.push_back(TravelData());
    }
    
    void add_dialog() {
        dialogs.push_back(DialogData());
    }
    
    void run(std::vector<std::string>& message_log)
    {
        for(auto& t: travels)
        {
            message_log.push_back("Traveled to another district");
        }
        
        // Player can escape any interaction if he manages to enter travel point at that turn
        // TODO: Maybe just clear other collections when we add a travel? But check is better for debugging
        if (travels.size() == 0) {
            for(auto& d: dialogs)
            {
                
            }
        }
        
        travels.clear();
        dialogs.clear();
    }
};


#endif /* interactions_h */
