//
//  quadtree.hpp
//  7drl-violation
//
//  Stolen on the web, modified
//
//  Copyright © 2016 Max Philippov
//

#ifndef quadtree_h
#define quadtree_h

#include <memory>
#include <vector>

#include "position.hpp"

namespace quadtree
{
    template <typename T>
    struct Data
    {
        Position pos;
        std::unique_ptr<T> load;

        Data(Position pos = Position(), std::unique_ptr<T> data = nullptr):
        pos(pos), load(data) {};
    };

    template <class T>
    class Quadtree
    {
    private:
        // Maybe replace with array
        std::unique_ptr<Quadtree> nw;
        std::unique_ptr<Quadtree> ne;
        std::unique_ptr<Quadtree> sw;
        std::unique_ptr<Quadtree> se;

        Bounds bounds;

        std::vector< Data<T> > objects;

        unsigned int capacity;
    public:
        Quadtree(Bounds bounds, unsigned int capacity = 4) :
        nw(nullptr),
        ne(nullptr),
        sw(nullptr),
        se(nullptr),
        bounds(bounds),
        capacity(capacity) {}

        auto subdivide()
        {
            auto minx = bounds.minx;
            auto maxx = bounds.maxx;
            auto miny = bounds.miny;
            auto maxy = bounds.maxy;
            auto midx = (bounds.maxx - bounds.minx) / 2 + bounds.minx;
            auto midy = (bounds.maxy - bounds.miny) / 2 + bounds.miny;

            auto nw_bounds = Bounds{ minx, miny, midx, midy };
            nw = std::make_unique<Quadtree>(nw_bounds);

            auto ne_bounds = Bounds{ midx, miny, maxx, midy };
            ne = std::make_unique<Quadtree>(ne_bounds);

            auto sw_bounds = Bounds{ minx, midy, midx, maxy };
            sw = std::make_unique<Quadtree>(sw_bounds);

            auto se_bounds = Bounds{ midx, midy, maxx, maxy };
            se = std::make_unique<Quadtree>(se_bounds);
        }

        auto insert(Data<T> d)
        {
            if (!bounds.contains(d.pos))
            {
                return false;
            }

            if (objects.size() < capacity)
            {
                objects.push_back(d);
                return true;
            }

            if (nw == nullptr)
            {
                subdivide();
            }

            if (nw->insert(d))
            {
                return true;
            }
            if (ne->insert(d))
            {
                return true;
            }
            if (sw->insert(d))
            {
                return true;
            }
            if (se->insert(d))
            {
                return true;
            }

            return false;
        }

        auto query_range(Bounds range) const
        {
            auto in_range = std::vector<Data<T>>();

            if (!bounds.intersects(range))
            {
                return in_range;
            }

            auto it = std::copy_if(std::begin(objects),
                                   std::end(objects),
                                   std::begin(in_range),
                                   [&range](auto const& o) {
                                       return range.contains(o.pos);
                                   });

            in_range.resize(std::distance(std::begin(in_range), it));

            if (nw == nullptr)
            {
                return in_range;
            }
            
            std::vector< Data<T> > temp = nw->query_range(range);
            in_range.insert(in_range.end(), temp.begin(), temp.end());
            
            temp = ne->query_range(range);
            in_range.insert(in_range.end(), temp.begin(), temp.end());
            
            temp = sw->query_range(range);
            in_range.insert(in_range.end(), temp.begin(), temp.end());
            
            temp = se->query_range(range);
            in_range.insert(in_range.end(), temp.begin(), temp.end());
            
            return in_range;
        }
    };
}

#endif /* quadtree_h */
