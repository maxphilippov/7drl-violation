//
//  interface.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef interface_h
#define interface_h

#include <ncurses.h>

#include <string>

bool confirmation_screen(MapSize const& half_screen_size, std::string const& message)
{
    clear();
    mvprintw(half_screen_size.height, 3, message.c_str());
    mvprintw(half_screen_size.height + 2, 3, "`y` to confirm, any key to cancel");
    refresh();

    auto input = getch();
    
    return input == 'y';
}

class WindowHandler
{
    WINDOW* ptr;
public:
    WindowHandler(Bounds const& b) :
    ptr(newwin(b.maxy - b.miny, b.maxx - b.minx, b.miny, b.minx))
    {
        wrefresh(ptr);
    }
    ~WindowHandler() { delwin(ptr); }
    
    WINDOW* raw() const { return ptr; }
};

#endif /* interface_h */
