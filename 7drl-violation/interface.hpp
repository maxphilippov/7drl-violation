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

class WindowHandler
{
    WINDOW* ptr;
public:
    WindowHandler(Bounds const& b):
    ptr(newwin(b.maxy - b.miny, b.maxx - b.minx, b.miny, b.minx)) {}
    ~WindowHandler() {
        wrefresh(ptr);
        delwin(ptr);
    }

    WINDOW* raw() const { return ptr; }
};

bool confirmation_screen(MapSize const& screen, std::string const& message)
{
        auto b = Bounds{ 0, 0, screen.width, screen.height };
        auto w = WindowHandler{b};
        mvwprintw(w.raw(), 0, 3, message.c_str());
        mvwprintw(w.raw(), 2, 3, "`y` to confirm, any key to cancel");

    auto input = wgetch(w.raw());

    return input == 'y';
}

#endif /* interface_h */
