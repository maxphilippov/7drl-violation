//
//  interactions.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef interactions_h
#define interactions_h

#include <vector>
#include <map>

#include "dialog.hpp"
#include "interface.hpp"

struct TravelData
{
    int destination_id;
};

struct DialogData
{
    DialogNode root;
};

auto police_officer_interaction()
{
    auto root = DialogNode {
        "Madam, can I check your id, please?",
        DialogNode::Replies {
            {
                "Cooperate", DialogNode {
                    "Thank you, this might take a moment", DialogNode::Replies {}
                }
            }
        }
    };
    
    return root;
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
        auto data = TravelData{
            destination_id
        };
        
        travels.push_back(data);
        // Player can escape any interaction if she manages to enter travel point at that turn
        dialogs.clear();
    }
    
    // Not like there won't be other dialogs, but gonna use that for POs only right now
    void add_dialog() {
        auto data = DialogData {
            police_officer_interaction()
        };
        dialogs.push_back(data);
    }
    
    void run(MapSize const& screen_size, std::vector<std::string>& message_log)
    {
        for(auto& t: travels)
        {
            message_log.push_back("Traveled to another district");
        }
        
        for(auto& d: dialogs)
        {
            render_dialog(screen_size, d.root);
        }
        
        travels.clear();
        dialogs.clear();
    }
};


#endif /* interactions_h */
