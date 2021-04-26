#include <thread>
#include <vector>
#include <condition_variable>
#include <queue>
#include "Event.h"
#include "ActiveObjects.h"
#include "./Units/Follower.h"
#include "./Units/Trap.h"
#ifndef WORKER_H
#define WORKER_H
class Worker
{
private:
    int active_area_rows;
    int active_area_cols;
    ActiveObjects& objects;
    static std::queue<Event> event_queue;
    static std::condition_variable work_con;
    static std::mutex event_lock; // To stall worker
    static std::mutex queue_lock; // To make event queue atomic
    bool game_ended;
    void stop();
    bool player_made;
    bool paused;
public:
    Worker(ActiveObjects&, int, int);
    void init();
    void handle(Event);
    friend Worker operator+=(Worker& w, const Event e);
    Event pull();
    bool can_push();
    bool ended();
};
#endif