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
#include <sstream>

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
        for(auto const& t: travels)
        {
            std::stringstream ss;
            ss << "Traveled to district #" << t.destination_id;
            message_log.push_back(ss.str());
        }

        // Player can escape any interaction if she manages to enter travel point at that turn
        if (travels.empty()) {
            for(auto const& d: dialogs)
            {
                render_dialog(screen_size, d.root);
            }
        }
        travels.clear();
        dialogs.clear();
    }
};


#endif /* interactions_h */
