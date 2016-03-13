//
//  interface.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef interface_h
#define interface_h

#include "curses.hpp"

#include <string>

#include "map.hpp"
#include "position.hpp"
#include "interaction_types.hpp"
#include "help_message.hpp"

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

void render_log(MapSize const& screen, std::vector<std::string> const& message_log)
{
    if (!message_log.empty()) {
        auto height = 7;
        auto y_offset = 2;

        auto end = std::crend(message_log);

        auto i = 0;

        for(auto it = std::rbegin(message_log); it != end && i != height; ++it, ++i) {
            mvprintw(screen.height + y_offset, 3, it->c_str());
            y_offset += 1;
        }
    }
}

bool confirmation_screen(MapSize const& screen, std::string const& message)
{
    const auto b = Bounds{ 0, 0, screen.width, screen.height };
    const auto w = WindowHandler{ b };
    const auto x_offset = 3;
    const auto y_offset = 6;
    mvwprintw(w.raw(), y_offset, x_offset, message.c_str());
    mvwprintw(w.raw(), y_offset + 2, x_offset, "`y` to confirm, any key to cancel");

    auto input = wgetch(w.raw());

    return input == 'y';
}

auto dialog_controls = std::unordered_set<int> {
    'j', 'k', ' '
};

void print_node(WINDOW * w, DialogNode const& d, int selected_node = 0)
{
    auto y_pos = 2;
    const auto x_pos = 2;
    wclear(w);
    mvwprintw(w, y_pos, x_pos, d.message.c_str());

    auto idx = 0;

    auto replies_count = static_cast<int>(d.replies.size());

    // FIXME: Add scrolling

    for(auto const& r: d.replies) {
        if (idx == selected_node ) wattron(w, A_REVERSE);
        y_pos += 2;
        mvwprintw(w, y_pos, x_pos + 1, r.first.c_str());
        if (idx == selected_node ) wattroff(w, A_REVERSE);
        ++idx;
    }

    mvwprintw(w, y_pos + 2, x_pos,
              (replies_count != 0) ?
              "j, k - pick line, <spacebar> - confirm" :
              "<Press spacebar to continue>");

    const auto choice = wgetch(w);

    switch(choice) {
        case 'j':
            if (replies_count != 0) {
                selected_node += 1;
                if (selected_node >= d.replies.size()) {
                    selected_node = 0;
                }
            }
            print_node(w, d, selected_node);
            break;
        case 'k':
            if (replies_count != 0) {
                selected_node -= 1;
                if (selected_node < 0) {
                    selected_node = replies_count - 1;
                }
            }
            print_node(w, d, selected_node);
            break;
        case ' ':
            if (replies_count != 0) {
                auto next_node = d.replies.at(selected_node).second;
                next_node.action();
                if (!(next_node.message.empty() && next_node.replies.empty())) {
                    print_node(w, next_node);
                }
            }
            break;
        default:
            print_node(w, d, selected_node);
            break;
    }
}

void render_dialog(MapSize const& screen, DialogNode const& root)
{
    const auto b = Bounds{ 0, screen.height, screen.width, screen.height };
    const auto w = WindowHandler{ b };
    
    print_node(w.raw(), root);
}

auto run_dialogs(MapSize const& screen_size,
                 std::vector<DialogNode> const& dialogs)
{

    for(auto const& d: dialogs)
    {
        render_dialog(screen_size, d);
    }
}

auto prompt(std::string const& message)
{
    echo();
    printw(message.c_str());
    char result[80];
    getstr(result);
    noecho();
    return std::string{ result };
}


#endif /* interface_h */
