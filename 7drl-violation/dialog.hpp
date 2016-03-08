//
//  dialog.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef dialog_h
#define dialog_h

#include <ncurses.h>

#include "interface.hpp"

struct DialogNode
{
    typedef std::map<std::string, DialogNode> Replies;
    std::string message;
    Replies replies;
};

void print_node(WINDOW * w, DialogNode const& d)
{
    auto y_pos = 2;
    auto x_pos = 2;
    mvwprintw(w, y_pos, x_pos, d.message.c_str());
    
    for(auto& r: d.replies) {
        y_pos += 2;
        mvwprintw(w, y_pos, x_pos + 1, r.first.c_str());
    }
    
    wrefresh(w);

    auto choice = getch();
}

void render_dialog(MapSize const& screen, DialogNode const& root)
{
    auto b = Bounds { 0, 0, screen.width, screen.height };
    auto w = WindowHandler {b};
    
    print_node(w.raw(), root);
}


#endif /* dialog_h */
