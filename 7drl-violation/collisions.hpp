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

struct AABB
{
    int minx, miny;
    int maxx, maxy;
    
    bool inside(Position const& pos) const
    {
        return pos.x >= minx && pos.y >= miny && pos.x < maxx && pos.y < maxy;
    }
};

struct Velocity
{
    int x;
    int y;
};

struct PhysicsData
{
    Position pos;
    Velocity vel;
};

class CollisionManager
{
    std::vector<PhysicsData> objects;
    std::unordered_map<int, InteractionType> interactables;
public:
    // Returns player position, temporary?
    Position update(Position const& player, MapSize const& bounds, InteractionQueue& interactions)
    {
        // Update all entities positions
        for(auto& m: objects) {
            // FIXME: How to make a copy and not leak
        }
        
        // On every collision generate an interaction
        
        auto pos = player.x + player.y * bounds.width;
        auto interaction = interactables.find(pos);
        if (interaction != std::end(interactables)) {
            interactions.add(interaction->second);
        }
        
        return player;
    }
    
    void change_velocity(int id)
    {
        
    }
    
    void teleport(int id, Position pos)
    {
        
    }
    
    void add_moving_entity(int id, Position pos, Velocity vel)
    {
        objects.push_back(PhysicsData{pos, vel});
    }
    
    std::vector<PhysicsData> get_in_range(Position const& player, MapSize const& half_screen) const
    {
        auto bounds = AABB{
            player.x - half_screen.width,
            player.y - half_screen.height,
            player.x + half_screen.width,
            player.x + half_screen.height
        };
        
        auto r = std::vector<PhysicsData>(objects.size());
        
        auto it = std::copy_if(std::begin(objects),
                               std::end(objects),
                               std::begin(r),
                               [&bounds](PhysicsData const& d) { return bounds.inside(d.pos); } );
        
        r.resize(std::distance(r.begin(), it));
        
        return r;
    }
};

#endif /* collisions_h */
