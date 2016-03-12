//
//  help_message.hpp
//  7drl-violation
//
//  Created by Max Philippov on 12/03/16.
//  Copyright © 2016 AGNMTLT. All rights reserved.
//

#ifndef help_message_h
#define help_message_h

#include <sstream>

#include "map.hpp"
#include "symbols.hpp"

std::string build_help_message() {
    auto ss = std::ostringstream{};

    ss << "\nGeneral:\n\t" << "q - exit\n\t" << "? - help\n";
    ss << "\nMovement:\n\t" << "h - left\n\t" << "l - right\n\t" << "j - down\n\t" << "k - top\n" << "spacebar - skip turn\n";
    ss << "\nItems: \n\t" << "t - phone \n";

    ss << "\nLegend:";

    ss << "\n\t" << symbols::actors.at(0) << " - player";

    ss << "\n\t" << symbols::actors.at(1) << " - police officer";

    ss << "\n\t" << symbols::map_tiles.at(MapTile::Empty) << " - street";

    ss << "\n\t" << symbols::map_tiles.at(MapTile::Road) << " - road";

    ss << "\n\t"  << symbols::map_tiles.at(MapTile::Wall) << " - wall";

    ss << "\n\t"  << symbols::map_tiles.at(MapTile::Door) << " - door";

    ss << "\n\t"  << symbols::map_tiles.at(MapTile::Crowd) << " - crowd";

    ss << "\nTips:";

    ss << "\nID is the key. You have one ID attached to you, but obviously it's an ID of a criminal. So you need a magical phone with a new ID attached, that should work for a while.";
    ss << "\n\nCrowds are like shadows, if you enter a crowd police will lose direct sight of you.";

    ss << "\n\nPress any key to exit";
    
    return ss.str();
}

#endif /* help_message_h */
