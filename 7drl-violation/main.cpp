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

#include <iostream>

#include <stdlib.h>

#include "curses.hpp"

#include "dialog.hpp"
#include "crowds.hpp"
#include "symbols.hpp"
#include "map.hpp"
#include "city.hpp"
#include "collisions.hpp"
#include "interface.hpp"
#include "police.hpp"
#include "position.hpp"
#include "timeline.hpp"

#include "game_state.hpp"

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

    std::vector<std::string> message_log;
public:
    Game(int width, int height) :
    screen_size{ width, height },
    half_size{ width / 2, height / 2 },
    player_id{ 0 },
    city{ MapSize { 512, 512 }, 5 },
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
        clear();
        printw("Thanks for playing Violation. See you soon, ma'am.");
        refresh();
        getch();

        curs_set(TRUE);
        endwin();

        refresh();
    }
public:
    void run()
    {
        std::vector<PoliceAlert> police_alerts;
        std::vector<identity_id_type> checked_ids;
        std::vector<district_id_type> travels;
        std::vector<DialogNode> dialogs = { intro_dialog() };

        std::vector<ActorCollisionInfo> collisions_info;

        police_alerts.reserve(32);

        GameState state{
            time,
            travels
        };
        auto player = city.change_district({}, 0);

        player_id = collisions.add_moving_entity(player.pos);

        police_alerts.push_back(police_alerts::android_murder(player_real_identity.id, player));

        auto input = '\0';

        while(!state.is_done()) {
            auto start = std::chrono::high_resolution_clock::now();
            clear();

            if (exit_buttons.count(input) > 0) {
                auto exit = confirmation_screen(screen_size, "Are you sure you want to exit?");
                if (exit) {
                    break;
                }
            }

            auto level_bounds = city.bounds();

            auto turn_counter = time.current_turn();

            auto current_district_id = city.get_current_district_id();
            auto neighbour_districts = city.get_neighbour_districts(current_district_id);

            auto pos = player.pos;
            auto player_interest = Bounds {
                std::max(pos.x - screen_size.width, 0),
                std::max(pos.y - screen_size.height, 0),
                std::min(pos.x + screen_size.width, level_bounds.width),
                std::min(pos.y + screen_size.height, level_bounds.height)
            };

            if (actions.count(input) != 0) {

                crowds.update(player_interest, city.map(), turn_counter);

                auto crowd_map = crowds.map();

                city.update(player_interest, crowd_map, turn_counter);

                // Produce velocity for movement orders
                police.update(player.pos, player_interest, collisions, city, turn_counter);

                auto pvel = Velocity{
                    (input == 'l') ? 1 : (input == 'h') ? -1 : 0,
                    (input == 'j') ? 1 : (input == 'k') ? -1 : 0
                };

                collisions.change_velocity(player_id, pvel);

                // Produces collisions information
                collisions.update(player_interest, city, collisions_info);

                auto player_pos_data = collisions.get_position(player_id);
                if (!player_pos_data.first) throw "Unable to find player position, it's a bug";
                player.pos = player_pos_data.second;

                // Adds interaction for completed jobs and probably creates something
                // like CityChange with data on what to change in city before next turn
                time.update(police, police_alerts, checked_ids);

                state.record_checked_ids(checked_ids);
            } else {
                switch(input) {
                    case '?':
                        render_help();
                        break;
                    case 't':
                    {
                        auto pd = phone_user_interface(state,
                                                       police_alerts,
                                                       player,
                                                       neighbour_districts,
                                                       turn_counter);
                        dialogs.push_back(pd);
                    }
                        break;
                    case 'e':
                    {
                        auto d = tile_to_interaction(city.get(player.pos),
                                                     player,
                                                     state,
                                                     police_alerts,
                                                     neighbour_districts,
                                                     current_district_id,
                                                     turn_counter);
                        dialogs.push_back(d);
                    }
                        break;
                }
            }

            for (auto const& c: collisions_info) {
                if (c.first == player_id || c.second == player_id) {
                    dialogs.push_back(
                        police_officer_interaction(state, police, police_alerts, player)
                    );
                    // If we don't break that's gonna turn into a bunch of dialogs
                    // which actually happen in parallel
                    break;
                }
            }

            render(player.pos, level_bounds);

            render_log(screen_size, message_log);

            police.record_crimes(police_alerts, message_log, turn_counter);

            if (state.discharge() == 0) {
                dialogs.push_back(no_charge_dialog(state));
            }

            if (state.is_in_jail()) {
                dialogs.push_back(prison_dialog(state));
            }

            if (state.is_won()) {
                dialogs.push_back(outro_dialog(state, screen_size));
            }

            run_dialogs(screen_size, dialogs);

            auto next_district = travels.empty() ? -1 : travels.back();

            if (next_district != -1) {
                player = city.change_district(player, next_district);
                auto new_size = city.bounds();
                crowds.resize(new_size);
                collisions.restart();
                player_id = collisions.add_moving_entity(player.pos);
                police.restart(collisions, player_interest);
                time.skip_time({ 1.5f });
            }

            refresh();

            dialogs.clear();
            travels.clear();
            police_alerts.clear();
            collisions_info.clear();
            checked_ids.clear();

            auto elapsed = std::chrono::high_resolution_clock::now() - start;

            input = getch();
            // For simulation testing
            // input = ' ';
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
    Game g(80, 20);
    
    g.run();
    
    return 0;
}
