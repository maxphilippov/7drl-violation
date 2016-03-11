//
//  timeline.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef timeline_h
#define timeline_h

#include <queue>

#include "time.hpp"
#include "id.hpp"
#include "interaction_types.hpp"

struct PurchaseCheck
{
    IDData data;
    WorldPosition pos;
    int price;
    int finish_by_turn;
};

struct IDCheck
{
    IDData data;
    WorldPosition pos;
    int finish_by_turn;
};

class Timeline
{
    static const Hours purchase_check_default_time;
    static const Hours id_check_default_time;

    int turn_counter;
    std::queue<PurchaseCheck> purchases_checks;
    std::queue<IDCheck> ID_checks;
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
    
    void add_purchase_check(IDData id,
                            WorldPosition pos,
                            int price,
                            Hours time_to_finish = purchase_check_default_time)
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

    void add_id_check(IDData id,
                      WorldPosition pos,
                      Hours time_to_finish = id_check_default_time)
    {
        auto c = IDCheck {
            id,
            pos,
            turn_counter + static_cast<int>(time_to_finish)
        };

        ID_checks.push(c);
    }
    
    void update(std::vector<PoliceAlert> & police_alerts)
    {
        if (purchases_checks.size() > 0) {
            auto job = purchases_checks.front();
            while(job.finish_by_turn == turn_counter) {
                purchases_checks.pop();
                if (job.data.balance - job.price < 0) {
                    police_alerts.push_back(PoliceAlert {
                        job.data,
                        job.pos,
                        // FIXME: Violation level
                        3
                    });
                }
                job = purchases_checks.front();
            }
        }
        if (ID_checks.size() > 0) {
            auto job = ID_checks.front();
            while(job.finish_by_turn == turn_counter) {
                ID_checks.pop();
                // FIXME: Check police records
                job = ID_checks.front();
            }
        }
        next_turn();
    }
};

const Hours Timeline::purchase_check_default_time = Hours { 1.5f };
const Hours Timeline::id_check_default_time = Hours { 12.0f };

#endif /* timeline_h */
