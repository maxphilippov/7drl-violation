//
//  timeline.hpp
//  7drl-violation
//
//  Copyright © 2016 Max Philippov
//

#ifndef timeline_h
#define timeline_h

#include <queue>

struct Job
{
    int turn_to_finish;
};

class Timeline
{
    int turn_counter;
    std::queue<Job> job_queue;
    // TODO: Add job pool
public:
    Timeline() : job_queue()
    {
        turn_counter = 1;
    }
    int current_turn() const
    {
        return turn_counter;
    }
    
    void add_job(int time_to_finish)
    {
        auto job = Job {
            turn_counter + time_to_finish
        };
        job_queue.push(job);
    }
    
    void update(InteractionQueue& interactions)
    {
        if (job_queue.size() > 0) {
            auto job = job_queue.front();
            while(job.turn_to_finish == turn_counter) {
                job_queue.pop();
                interactions.add(InteractionType::JobComplete);
                job = job_queue.front();
            }
        }
    }
    
    int next_turn()
    {
        return turn_counter += 1;
    }
};

#endif /* timeline_h */
