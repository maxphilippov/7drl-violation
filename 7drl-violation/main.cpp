//
//  main.cpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#include <algorithm>
#include <vector>
#include <unordered_set>

#include <stdlib.h>

#include <ncurses.h>

#include "symbols.hpp"
#include "map.hpp"
#include "city.hpp"
#include "collisions.hpp"
#include "interactions.hpp"
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

class Game
{
private:
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
    Game(int width, int height) : half_size{width / 2, height / 2}
    {
        initscr();
        noecho();
        curs_set(FALSE);
        message_log.reserve(256);
    }
    ~Game()
    {
        printw("Thanks for playing Violation. See you soon, mam.");
        // FIXME: Commented for tests
        // getch();
        endwin();
    }
    
    void intro_sequence()
    {
        interactions.add_travel(0);
        time.add_job(4);
        
        player_id = collisions.add_moving_entity(Position{32, 32});
        
        printw("Your master is dead, the blood is on your hands, hurry up, they are looking for a female android");
        
        getch();
        clear();
    }
    
    void loop(Position const& player)
    {
        int input;
        auto nextPosition = player;
        do {
            clear();
            
            auto level_bounds = city.bounds();
            
            Velocity pVelocity;
            // should push velocity to CollisionsManager to let it later handle things
            pVelocity.x = (input == 'l') ? 1 : (input == 'h') ? -1 : 0;
            pVelocity.y = (input == 'j') ? 1 : (input == 'k') ? -1 : 0;
            
            collisions.change_velocity(player_id, pVelocity);

            // Produce velocity for movement orders
            police.update(nextPosition, collisions);
            
            // Produce interactions list, should be sorted by priority
            // Like a message from PO is more important than your interaction with civilians
            nextPosition = collisions.update(nextPosition, level_bounds, interactions);
            
            auto cells = city.map();
            
            render(nextPosition, cells, level_bounds);
            // OK, now the interface
            
            // Adds interaction for completed jobs and probably creates something
            // like CityChange with data on what to change in city before next turn
            time.update(interactions);
            
            // Maybe we don't need a class for that, just make a simple vector
            // and non-member function to execute every interaction
            interactions.run(message_log);
            
            render_log();
            
            city.update(nextPosition);
            
            refresh();
            
            time.next_turn();
            
            input = getch();
        } while(exit_buttons.count(input) == 0);
    }
private:
    void render(Position const& player, MapCells const& map, MapSize const& bounds) const
    {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_BLUE, COLOR_WHITE);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        
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
                mvprintw(y, x, symbols::mapTiles.at(code).c_str());
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
            mvprintw(y, x, symbols::actors.at(2).c_str());
        }
        attroff(COLOR_PAIR(2));
        
        // Player
        attron(COLOR_PAIR(3));
        mvprintw(half_size.height, half_size.width, symbols::actors.at(0).c_str());
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
    auto g = Game(80, 60);
    
    g.intro_sequence();
    
    g.loop(Position{32,32});
    
    return 0;
}
