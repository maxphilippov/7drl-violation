//
//  interface.hpp
//  7drl-violation
//
//  Created by Max Philippov on 08/03/16.
//  Copyright © 2016 AGNMTLT. All rights reserved.
//

#ifndef interface_h
#define interface_h

#include <ncurses.h>

#include <string>

bool confirmation_screen(MapSize const& half_screen_size, std::string const& message)
{
    mvprintw(half_screen_size.height, 3, message.c_str());
    mvprintw(half_screen_size.height + 2, 3, "`y` to confirm, any key to cancel");
    refresh();

    auto input = getch();
    
    return input == 'y';
}

#endif /* interface_h */
