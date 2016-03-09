//
//  collisions.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef collisions_h
#define collisions_h

#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>

#include "map.hpp"
#include "interactions.hpp"
#include "position.hpp"
#include "city.hpp"

std::unordered_set<MapTile> unpassable_tiles = {
    MapTile::Wall
};

class CollisionManager
{
    std::vector<Position> positions;
    std::vector<Velocity> velocities;

    int last_id;
    std::vector<int> object_ids;
public:
    typedef std::vector<Position>::size_type id_type;

    CollisionManager() :
    last_id(0)
    {
        object_ids.reserve(256);
        positions.reserve(64);
        velocities.reserve(64);
    }
    
    // Returns player position, temporary?
    void update(Position const& around_point, // Point of interest for us, maybe chagne to bounds?
                CityManager const& city,
                InteractionQueue& interactions)
    {
        auto level_bounds = city.bounds();
        // Update all entities positions (actually should update just nearest to the player)
        auto new_positions = std::vector<Position>(positions.size());
        
        std::transform(std::begin(positions),
                       std::end(positions),
                       std::begin(velocities),
                       std::begin(new_positions),
                       [&level_bounds, &city](Position const& p, Velocity const& v) {
                           auto new_pos = Position{
                               std::max(0, std::min(p.x + v.x, level_bounds.width - 1)),
                               std::max(0, std::min(p.y + v.y, level_bounds.height - 1))
                           };

                           auto tile = city.get(new_pos);

                           return (unpassable_tiles.count(tile) == 0) ? new_pos : p;
                       });
        
        // TODO: Actually check collisions
        // For collision checking we might need to sort a vector,
        // that makes the whole id thing look bad
        // Looks like I gotta change how data is stored,
        // Cause we'll need to check object priorities when two entities try to enter same cell
        // Police officer > Player > Others
        
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
    
    bool raycast(MapCells const& cells, Position const& from, Position const& to) const
    {
        // Draw simple line and check if there are no walls or crowds on the way
        return true;
    }
    
    void drop_object(id_type id) {
        if(positions.size() > id) {
            positions.erase(std::begin(positions) + id - 1);
            velocities.erase(std::begin(velocities) + id - 1);
        }
    }

    // Return false if there's no such object registered for collisions
    // which means we disposed it already
    bool change_velocity(id_type id, Velocity vel)
    {
        velocities.at(id) = vel;
        return true;
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
        
        r.resize(std::distance(std::begin(r), it));
        
        return r;
    }
};

#endif /* collisions_h */
