//
//  collisions.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef collisions_h
#define collisions_h

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <iterator>

#include "map.hpp"
#include "interactions.hpp"
#include "position.hpp"

class CollisionManager
{
    std::vector<Position> positions;
    std::vector<Velocity> velocities;
public:
    CollisionManager()
    {
        positions.reserve(64);
        velocities.reserve(64);
    }
    
    // Returns player position, temporary?
    Position update(Position const& player, MapSize const& level_bounds, InteractionQueue& interactions)
    {
        // Update all entities positions
        for(auto const& v: velocities) {
            // FIXME: How to make a copy and not leak
            
            
        }
        
        auto new_positions = std::vector<Position>(positions.size());
        
        std::transform(std::begin(positions),
                       std::end(positions),
                       std::begin(velocities),
                       std::begin(new_positions),
                       [&level_bounds](Position const& p, Velocity const& v) {
                           return Position{
                               std::max(0, std::min(p.x + v.x, level_bounds.width - 1)),
                               std::max(0, std::min(p.y + v.y, level_bounds.height - 1))
                           };
                       });
        
        std::swap(positions, new_positions);
        
        // TODO: Find a better way to nullify every velocity
        std::transform(std::begin(velocities),
                       std::end(velocities),
                       std::begin(velocities),
                       [](Velocity const& v) {
                           return Velocity{ 0, 0 };
                       });
        
        // TODO: On every collision generate an interaction

        return positions.at(0);
    }
    
    void drop_object(int id) {
        if(positions.size() > id) {
            positions.erase(std::begin(positions) + id - 1);
            velocities.erase(std::begin(velocities) + id - 1);
        }
    }
    
    void change_velocity(unsigned long id, Velocity vel)
    {
        velocities.at(id) = vel;
    }
    
    void teleport(int id, Position pos)
    {
        
    }
    
    unsigned long add_moving_entity(Position pos, Velocity vel = Velocity{0, 0})
    {
        auto id = positions.size();
        
        positions.push_back(pos);
        velocities.push_back(vel);
        
        return id;
    }
    
    std::vector<Position> get_in_range(Position const& player, MapSize const& half_screen) const
    {
        auto bounds = Bounds{
            player.x - half_screen.width,
            player.y - half_screen.height,
            player.x + half_screen.width,
            player.x + half_screen.height
        };
        
        auto r = std::vector<Position>(positions.size());
        
        auto it = std::copy_if(std::begin(positions),
                               std::end(positions),
                               std::begin(r),
                               [&bounds](Position const& p) { return bounds.inside(p); } );
        
        r.resize(std::distance(r.begin(), it));
        
        return r;
    }
};

#endif /* collisions_h */
