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
#include "id.hpp"
#include "interface.hpp"
#include "map.hpp"

struct TravelData
{
    int destination_id;
};

struct DialogData
{
    DialogNode root;
};

struct PoliceAlert
{
    IDData id;
    Position pos;
    int violation_level;
};

auto run_travels(std::vector<TravelData> const& travels, std::vector<std::string> & message_log)
{
    for(auto const& t: travels)
    {
        std::ostringstream ss;
        ss << "Traveled to district #" << t.destination_id;
        message_log.push_back(ss.str());
    }
}

auto run_dialogs(MapSize const& screen_size, std::vector<DialogData> const& dialogs)
{

    for(auto const& d: dialogs)
    {
        render_dialog(screen_size, d.root);
    }
}


#endif /* interactions_h */
