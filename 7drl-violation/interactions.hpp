//
//  interactions.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef interactions_h
#define interactions_h

#include <vector>

#include "interaction_types.hpp"
#include "interface.hpp"
#include "map.hpp"

auto run_travels(std::vector<TravelData> const& travels)
{
    for(auto const& t: travels)
    {
        return t.destination_id;
    }

    return -1;
}

auto run_dialogs(MapSize const& screen_size, std::vector<DialogData> const& dialogs)
{

    for(auto const& d: dialogs)
    {
        render_dialog(screen_size, d.root);
    }
}


#endif /* interactions_h */
