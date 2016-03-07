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
    typedef std::vector<Position>::size_type id_type;

    CollisionManager()
    {
        positions.reserve(64);
        velocities.reserve(64);
    }
    
    // Returns player position, temporary?
    void update(Position const& player, MapSize const& level_bounds, InteractionQueue& interactions)
    {
        // Update all entities positions (actually should update just nearest to the player)
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
        
        // TODO: Actually check collisions
        // For collision checking we might need to sort a vector,
        // that makes the whole id thing look bad
        
        std::swap(positions, new_positions);
        
        // TODO: Find a better way to nullify every velocity
        std::transform(std::begin(velocities),
                       std::end(velocities),
                       std::begin(velocities),
                       [](Velocity const& v) {
                           return Velocity{ 0, 0 };
                       });
        
        // TODO: On every collision generate an interaction
    }
    
    void drop_object(id_type id) {
        if(positions.size() > id) {
            positions.erase(std::begin(positions) + id - 1);
            velocities.erase(std::begin(velocities) + id - 1);
        }
    }
    
    void change_velocity(id_type id, Velocity vel)
    {
        velocities.at(id) = vel;
    }
    
    Position const& get_position(id_type id) const
    {
        return positions.at(id);
    }
    
    void teleport(id_type id, Position pos)
    {
        positions.at(id) = pos;
    }
    
    auto add_moving_entity(Position pos, Velocity vel = Velocity{0, 0})
    {
        auto id = positions.size();
        
        positions.push_back(pos);
        velocities.push_back(vel);
        
        return id;
    }
    
    auto get_in_range(Position const& player, MapSize const& half_screen) const
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
