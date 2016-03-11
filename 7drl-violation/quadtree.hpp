//
//  quadtree.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef quadtree_h
#define quadtree_h

#include <memory
#include <vector>

#include "position.hpp"

template <typename T>
struct Data
{
    Position pos;
    T* load;

    Data(Position pos = Position(), T* data = nullptr): pos(pos), load(data){};
};


template <class T>
class Quadtree
{
private:
    //4 children
    std::unique_ptr<Quadtree> nw;
    std::unique_ptr<Quadtree> ne;
    std::unique_ptr<Quadtree> sw;
    std::unique_ptr<Quadtree> se;

    Bounds boundary;

    std::vector< Data<T> > objects;

    int CAPACITY;
public:
    Quadtree<T>();
    Quadtree<T>(Bounds boundary);

    ~Quadtree();

    bool insert(Data<T> d);
    void subdivide();
    std::vector< Data<T> > queryRange(Bounds range);
};

template <class T>
Quadtree<T>::Quadtree() :
nw(nullptr),
ne(nullptr),
sw(nullptr),
se(nullptr)
{
    CAPACITY = 4;
}

template <class T>
Quadtree<T>::Quadtree(Bounds boundary)
nw(nullptr),
ne(nullptr),
sw(nullptr),
se(nullptr),
boundary(boundary)
{
    CAPACITY = 4;
}

template <class T>
void Quadtree<T>::subdivide()
{
    Position qSize = Position(boundary.halfSize.x, boundary.halfSize.y);
    Position qCentre = Position(boundary.centre.x - qSize.x, boundary.centre.y - qSize.y);
    nw = new Quadtree(Bounds(qCentre, qSize));

    qCentre = Position(boundary.centre.x + qSize.x, boundary.centre.y - qSize.y);
    ne = new Quadtree(Bounds(qCentre, qSize));

    qCentre = Position(boundary.centre.x - qSize.x, boundary.centre.y + qSize.y);
    sw = new Quadtree(Bounds(qCentre, qSize));

    qCentre = Position(boundary.centre.x + qSize.x, boundary.centre.y + qSize.y);
    se = new Quadtree(Bounds(qCentre, qSize));
}

template <class T>
bool Quadtree<T>::insert(Data<T> d)
{
    if(!boundary.contains(d.pos))
    {
        return false;
    }

    if(objects.size() < CAPACITY)
    {
        objects.push_back(d);
        return true;
    }

    if(nw == nullptr)
    {
        subdivide();
    }

    if(nw->insert(d))
    {
        return true;
    }
    if(ne->insert(d))
    {
        return true;
    }
    if(sw->insert(d))
    {
        return true;
    }
    if(se->insert(d))
    {
        return true;
    }

    return false;
}

template <class T>
std::vector< Data<T> > Quadtree<T>::queryRange(Bounds range)
{
    std::vector< Data<T> > pInRange = std::vector< Data<T> >();

    if(!boundary.intersects(range))
    {
        return pInRange;
    }

    for(int i = 0; i < objects.size(); i++)
    {
        if(range.contains(objects.at(i).pos))
        {
            pInRange.push_back(objects.at(i));
        }
    }

    if(nw == nullptr)
    {
        return pInRange;
    }

    std::vector< Data<T> > temp = nw->queryRange(range);
    pInRange.insert(pInRange.end(), temp.begin(), temp.end());

    temp = ne->queryRange(range);
    pInRange.insert(pInRange.end(), temp.begin(), temp.end());

    temp = sw->queryRange(range);
    pInRange.insert(pInRange.end(), temp.begin(), temp.end());

    temp = se->queryRange(range);
    pInRange.insert(pInRange.end(), temp.begin(), temp.end());

    return pInRange;
}

#endif /* quadtree_h */
