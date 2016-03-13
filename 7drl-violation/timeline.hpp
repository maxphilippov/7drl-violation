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
#include "police.hpp"

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
    
    void update(PoliceManager & police,
                std::vector<PoliceAlert> & police_alerts,
                std::vector<identity_id_type> & checked_ids)
    {
        using namespace police_alerts;
        if (!purchases_checks.empty()) {
            auto job = purchases_checks.front();
            while(job.finish_by_turn == turn_counter) {
                purchases_checks.pop();
                if (job.data.balance - job.price < 0) {
                    police_alerts.push_back(purchase_balance_alert(job.data.id, job.pos));
                }
                job = purchases_checks.front();
            }
        }
        if (!ID_checks.empty()) {
            auto job = ID_checks.front();
            while(job.finish_by_turn == turn_counter) {
                ID_checks.pop();
                if (police.check_crime_history(job.data.id, job.pos) > 10) {
                    police_alerts.push_back(attempt_to_escape(job.data.id, job.pos));
                } else {
                    checked_ids.push_back(job.data.id);
                }
                job = ID_checks.front();
            }
        }
        next_turn();
    }
};

const Hours Timeline::purchase_check_default_time = Hours { 1.5f };
// FIXME:
const Hours Timeline::id_check_default_time = Hours { 0.5f };

#endif /* timeline_h */
