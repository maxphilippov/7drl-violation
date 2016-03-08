//
//  main.cpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#include <algorithm>
#include <sstream>
#include <vector>
#include <unordered_set>

#include <stdlib.h>

#include <ncurses.h>

#include "symbols.hpp"
#include "map.hpp"
#include "city.hpp"
#include "collisions.hpp"
#include "id.hpp"
#include "interactions.hpp"
#include "items.hpp"
#include "police.hpp"
#include "position.hpp"
#include "timeline.hpp"

const std::unordered_set<char> actions = {
    'h', 'j', 'k', 'l', // Movement
    ' ', // Stand still
};

const std::unordered_set<char> exit_buttons = {
    'q', 'Q'
};

std::string build_help_message() {
    auto ss = std::stringstream();
    
    ss << "\nGeneral:\n\t" << "q - exit\n\t" << "? - help\n";
    ss << "\nMovement:\n\t" << "h - left\n\t" << "l - right\n\t" << "j - down\n\t" << "k - top\n" << "spacebar - skip turn\n";
    ss << "\nItems: \n\t" << "t - phone \n";
    
    ss << "\nTips:";
    
    ss << "\nID is the key. You have one ID attached to you, but obviously it's an ID of a criminal. So you need a magical phone with a new ID attached, that should work for a while.";
    ss << "\n\nCrowds are like shadows, if you enter a crowd police will lose direct sight of you.";
    
    ss << "\n\nPress any key to exit";
    
    return ss.str();
}

const auto helpMessage = build_help_message();

void render_help()
{
    printw(helpMessage.c_str());
    
    getch();
}

class Game
{
private:
    const MapSize screen_size;
    const MapSize half_size;
    
    // TODO: Make a separate manager for player input
    unsigned long player_id;
    
    CityManager city;
    CollisionManager collisions;
    Timeline time;
    InteractionQueue interactions;
    PoliceManager police;
    
    std::vector<std::string> message_log;
public:
    Game(int width, int height) : screen_size{ width, height }, half_size{ width / 2, height / 2 }
    {
        initscr();
        noecho();
//        curs_set(FALSE);
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_BLUE, COLOR_WHITE);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        
        message_log.reserve(256);
    }
    
    ~Game()
    {
        printw("Thanks for playing Violation. See you soon, mam.");
        // FIXME: Commented for tests
        // getch();
        endwin();
    }
    
    void run()
    {
        interactions.add_travel(0);
        interactions.add_dialog();
        
        auto player_pos = Position{ 32, 32 };
        
        player_id = collisions.add_moving_entity(player_pos);
        
        printw("Your master is dead, the blood is on your hands, hurry up, they are looking for a female android");
        
        // Find a nearest bar and look for a smuggler
        // Don't pay for anything in public places unless you want police to know that
        // Pay for a fake ID with the money you got left
        
        getch();
        
        loop(player_pos);
    }
private:
    void loop(Position const& player)
    {
        auto nextPosition = player;
        auto input = '\0';
        do {
            clear();
            
            auto level_bounds = city.bounds();
            
            if(actions.count(input) != 0) {
                
                Velocity pVelocity;
                // should push velocity to CollisionsManager to let it later handle things
                pVelocity.x = (input == 'l') ? 1 : (input == 'h') ? -1 : 0;
                pVelocity.y = (input == 'j') ? 1 : (input == 'k') ? -1 : 0;
                
                collisions.change_velocity(player_id, pVelocity);
                
                // Produce velocity for movement orders
                police.update(nextPosition, collisions);
                
                // Produce interactions list, should be sorted by priority
                // Like a message from PO is more important than your interaction with civilians
                collisions.update(nextPosition, level_bounds, interactions);
                
                nextPosition = collisions.get_position(player_id);
                
                render(nextPosition, level_bounds);
                // OK, now the interface
                
                // Adds interaction for completed jobs and probably creates something
                // like CityChange with data on what to change in city before next turn
                time.update(interactions);
                
                // Maybe we don't need a class for that, just make a simple vector
                // and non-member function to execute every interaction
                interactions.run(screen_size, message_log);
                
                render_log();
                
                city.update(nextPosition);
                
                time.next_turn();
                
            } else if(input == '?') {
                render_help();
            // Temporary testing
            } else if(input == 't') {
                travel();
            } else {
                render(nextPosition, level_bounds);
                
                render_log();
            }

//            mvcur(0, 0, half_size.height + 1, half_size.width);
            
            refresh();
            
            // we can store int rewind_to_turn here and ask for input
            // only if(rewind_to_turn == time.current_turn())
            // --rewund_to_turn
            input = getch();
        } while(exit_buttons.count(input) == 0);
    }
    
    // TODO: Don't know where to put that, looks like an input manager
    void travel()
    {
        auto response = confirmation_screen(half_size, "Do you want to travel to x?");
    
        if (response) {
            message_log.push_back("Traveling to x");
            time.skip_time(Hours{5});
        } else {
            message_log.push_back("Travel canceled");
        }
    }
    
    void render(Position const& player, MapSize const& bounds) const
    {
        auto init_x = std::max(half_size.width - player.x, 0);
        auto init_y = std::max(half_size.height - player.y, 0);
        
        auto max_width = half_size.width + std::min(bounds.width - player.x, half_size.width);
        auto max_height = half_size.height + std::min(bounds.height - player.y, half_size.height);
        
        auto left = player.x - half_size.width;
        auto top = player.y - half_size.height;
        
        // Terrain
        attron(COLOR_PAIR(1));
        for(auto y = init_y; y < max_height; ++y) {
            for (auto x = init_x; x < max_width; ++x) {
                auto world_x = left + x;
                auto world_y = top + y;
                auto code = city.get(world_x, world_y);
                mvaddch(y, x, symbols::mapTiles.at(code));
            }
        }
        attroff(COLOR_PAIR(1));
        
        // Actors
        auto actor_positions = collisions.get_in_range(player, half_size);
        
        attron(COLOR_PAIR(2));
        for(auto pos: actor_positions) {
            // TODO: Color hostile actors with red background?
            // FIXME: Now renders an actor under player too
            auto x = pos.x - left;
            auto y = pos.y - top;
            mvaddch(y, x, symbols::actors.at(2));
        }
        attroff(COLOR_PAIR(2));
        
        // Player
        attron(COLOR_PAIR(3));
        mvaddch(half_size.height, half_size.width, symbols::actors.at(0));
        attroff(COLOR_PAIR(3));
    }
    
    void render_log() const
    {
        if (message_log.size() > 0) {
            int y_offset = 2;
            // TODO: Reverse logging
            for(auto& m: message_log) {
                mvprintw(half_size.height * 2 + y_offset, 3, m.c_str());
                y_offset += 1;
            }
        }
    }
};

int main(int argc, const char * argv[]) {
    // TODO: Reduce screen size to 20, 20 instead of any kind of vision control
    auto g = Game(80, 30);
    
    g.run();
    
    return 0;
}
