//
//  timeline.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef timeline_h
#define timeline_h

#include <queue>

#include "id.hpp"
#include "interactions.hpp"

const unsigned int turns_in_hour = 12;

class Hours
{
    float h;
public:
    Hours(float h) : h(h) {}
    // I'd like to make it return a Turns type, but
    // has to overload all operators, so it's int for the moment
    operator int() { return static_cast<int>(h * turns_in_hour); }
};

struct PurchaseCheck
{
    IDData data;
    Position pos;
    int price;
    int finish_by_turn;
};

struct IDCheck
{
    IDData data;
};

class Timeline
{
    int turn_counter;
    std::queue<PurchaseCheck> purchases_checks;
    std::queue<IDCheck> ID_checks;
    // TODO: Add job pool
public:
    Timeline()
    {
        turn_counter = 1;
    }

    auto skip_time(Hours t)
    {
        turn_counter += static_cast<int>(t);
    }
    
    auto current_turn() const
    {
        return turn_counter;
    }
    
    auto next_turn()
    {
        return turn_counter += 1;
    }
    
    void add_purchase_check(IDData id, Position pos, int price, Hours time_to_finish)
    {
        // We copy IDData here, cause
        // check is made against the balance you had after purchase
        // not at the time of the check
        auto c = PurchaseCheck {
            id,
            pos,
            price,
            turn_counter + static_cast<int>(time_to_finish)
        };
        
        purchases_checks.push(c);
    }
    
    void update(InteractionQueue& interactions)
    {
        if (purchases_checks.size() > 0) {
            auto job = purchases_checks.front();
            while(job.finish_by_turn == turn_counter) {
                purchases_checks.pop();
                if (job.data.balance - job.price < 0) {
                    interactions.add_police_alert(
                        //FIXME: Violation level
                        job.data, job.pos, 1
                    );
                }
                job = purchases_checks.front();
            }
        }
        next_turn();
    }
};

#endif /* timeline_h */
