//
//  main.cpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#include <algorithm>
#include <chrono>
#include <sstream>
#include <vector>
#include <unordered_set>

#include <stdlib.h>

#include "curses.hpp"

#include "dialog.hpp"
#include "crowds.hpp"
#include "symbols.hpp"
#include "map.hpp"
#include "city.hpp"
#include "collisions.hpp"
#include "interface.hpp"
#include "items.hpp"
#include "police.hpp"
#include "position.hpp"
#include "timeline.hpp"

#include "player_commands.hpp"

const std::unordered_set<char> actions = {
    'h', 'j', 'k', 'l', // Movement
    ' ', // Stand still
};

const std::unordered_set<char> exit_buttons = {
    'q', 'Q'
};

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
    physical_object_id_type player_id;

    CityManager city;
    CrowdManager crowds;
    // Processes movement
    CollisionManager collisions;
    // Processes over time events, maintains turn counter
    Timeline time;
    // Processes police officer tasks etc.
    PoliceManager police;
    InventoryManager items;

    std::vector<std::string> message_log;
public:
    Game(int width, int height) :
    screen_size{ width, height },
    half_size{ width / 2, height / 2 },
    player_id{ 0 },
    city{ MapSize { 512, 512 } },
    crowds{ city.bounds() }
    {
        initscr();
        noecho();
        curs_set(FALSE);
        if (has_colors()) {
            start_color();
            // Basic
            init_pair(1, COLOR_WHITE, COLOR_BLACK);
            // PO colors
            init_pair(2, COLOR_WHITE, COLOR_BLUE);
            // Player color
            init_pair(3, COLOR_GREEN, COLOR_BLACK);
            // Crowds
            init_pair(4, COLOR_BLACK, COLOR_WHITE);
        }

        message_log.reserve(256);
    }

    ~Game()
    {
        printw("Thanks for playing Violation. See you soon, ma'am.");
        // FIXME: Commented for tests
        // getch();

        curs_set(TRUE);
        endwin();

        refresh();
    }
public:
    void run()
    {
        std::vector<PoliceAlert> police_alerts;
        // A vector of travels is pretty stupid
        std::vector<district_id_type> travels;
        std::vector<DialogData> dialogs = {{ intro_dialog() }};

        std::vector<ActorCollisionInfo> collisions_info;

        PlayerInput input_manager{
            time,
            items,
            travels
        };

        police_alerts.reserve(32);

        auto player = city.change_district({}, 0);

        player_id = collisions.add_moving_entity(player.pos);

        auto input = '\0';
        while(true) {
            auto start = std::chrono::high_resolution_clock::now();
            clear();

            if (exit_buttons.count(input) > 0) {
                // FIXME: auto exit = confirmation_screen(screen_size, "Are you sure you want to exit?");
                auto exit = true; // autoexit for debugging
                if (exit) {
                    break;
                }
            }

            auto level_bounds = city.bounds();

            auto turn_counter = time.current_turn();

            auto current_district_id = city.get_current_district_id();
            auto neighbour_districts = city.get_neighbour_districts(current_district_id);

            if (actions.count(input) != 0) {

                auto pos = player.pos;
                auto player_interest = Bounds {
                    pos.x - screen_size.width, pos.y - screen_size.width,
                    pos.x + screen_size.height, pos.y + screen_size.height
                };

                auto pvel = Velocity{
                    (input == 'l') ? 1 : (input == 'h') ? -1 : 0,
                    (input == 'j') ? 1 : (input == 'k') ? -1 : 0
                };

                collisions.change_velocity(player_id, pvel);

                // Produce velocity for movement orders
                police.update(player.pos, player_interest, collisions, turn_counter);

                crowds.update(player_interest, city.map(), turn_counter);

                auto crowd_map = crowds.map();

                city.update(player_interest, crowd_map, turn_counter);

                // Produces collisions information
                collisions.update(player_interest, city, collisions_info);

                // FIXME: Should probably throw if we can't find a player,
                // that would mean there's a mess up in code
                player.pos = collisions.get_position(player_id).second;

                // Adds interaction for completed jobs and probably creates something
                // like CityChange with data on what to change in city before next turn
                time.update(police_alerts);
            } else {
                switch(input) {
                    case '?':
                        render_help();
                        break;
                    case 't':
                        auto pd = phone_user_interface(player, items.get_id(), input_manager, neighbour_districts, turn_counter);
                        dialogs.push_back({ pd });
                        break;
                }
            }

            for (auto const& c: collisions_info) {
                if (c.first == player_id || c.second == player_id) {
                    dialogs.push_back({
                        police_officer_interaction(player, items.get_id(), police)
                    });
                    // If we don't break that's gonna turn into a bunch of dialogs
                    // which actually happens in parallel
                    break;
                }
            }

            render(player.pos, level_bounds);

            render_log(screen_size, message_log);

            police.record_crimes(police_alerts, message_log);

            run_dialogs(screen_size, dialogs);

            auto next_district = travels.empty() ? -1 : travels.back();

            if (next_district != -1) {
                player = city.change_district(player, next_district);
                auto new_size = city.bounds();
                crowds.resize(new_size);
                collisions.restart();
                player_id = collisions.add_moving_entity(player.pos);
                police.restart();
                // FIXME: Change time constant
                time.skip_time({ 1.5f });
            }

            refresh();

            dialogs.clear();
            travels.clear();
            police_alerts.clear();
            collisions_info.clear();

            auto elapsed = std::chrono::high_resolution_clock::now() - start;

            // we can store int rewind_to_turn here and ask for input
            // only if(rewind_to_turn == time.current_turn())
            // --rewind_to_turn
            input = getch();
        }
    }
private:

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
                if (code == MapTile::Crowd) attron(COLOR_PAIR(4));
                mvaddch(y, x, symbols::map_tiles.at(code));
                if (code == MapTile::Crowd) attroff(COLOR_PAIR(4));
            }
        }
        attroff(COLOR_PAIR(1));

        // Actors
        auto actor_positions = collisions.get_in_range(player, half_size);

        attron(COLOR_PAIR(2));
        for(auto const& pos: actor_positions) {
            // TODO: Color hostile actors with red background?
            auto x = pos.x - left;
            auto y = pos.y - top;
            mvaddch(y, x, symbols::actors.at(1));
        }
        attroff(COLOR_PAIR(2));
        
        // Player
        attron(COLOR_PAIR(3));
        mvaddch(half_size.height, half_size.width, symbols::actors.at(0));
        attroff(COLOR_PAIR(3));
    }
};

int main(int argc, const char * argv[]) {
    // FIXME: Reduce screen size to 20, 20 instead of any kind of vision control
    Game g(80, 20);
    
    g.run();
    
    return 0;
}
