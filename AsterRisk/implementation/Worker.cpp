#include "../headers/Worker.h"
#include "../headers/Units/Unit.h"
#include "../headers/Units/Follower.h"
#include "../headers/Units/Trap.h"
#include "../headers/Units/Sweeper.h"
#include "../headers/ActiveObjects.h"
#include <ncurses.h> // Include ncurses so its symbol macros can be used
#include <mutex>
#include <iostream>
#include <algorithm>

std::queue<Event> Worker::event_queue;
std::condition_variable Worker::work_con;
std::mutex Worker::event_lock;
std::mutex Worker::queue_lock;

Worker::Worker(ActiveObjects& objects, int rows, int cols) : objects(objects)
{
    active_area_rows = rows - 2;
    active_area_cols = (cols - cols/3) - 2;
    // Game state flags
    player_made = false;
    paused = false;
    game_ended = false;
}

void Worker::handle(Event to_handle)
{
    try
    {
        int hp;

        // Don't accept any input besides make player and end game if player has not been created yet (prevents out_of_range exceptions)
        if(!player_made && to_handle.get_event_type() != EventType::Player_Make && to_handle.get_event_type() != EventType::Game_End) return;
        // Don't accept any input besides pause events if the game is paused
        if(paused && to_handle.get_event_type() != EventType::Game_Pause) return;
        // End the game if the player does not exist, or if there are no active units and the game has started
        if((!objects.has_player() && player_made) || (objects.is_empty() && player_made)) game_ended = true;
        if(game_ended) stop();
        switch(to_handle.get_event_type())
        {
            case EventType::Player_Make:
                {
                    Unit to_make = Unit(active_area_rows/2, active_area_cols/2, 'A', "player", 0, 5);
                    objects += to_make;
                    player_made = true;
                }
                break;
            case EventType::Follower_Make:
                {
                    if(to_handle.get_source_name().find("super") != std::string::npos) hp = 5;
                    else hp = 1;
                    Follower to_make = Follower(to_handle.get_x(), to_handle.get_y(), 'V', "enemy_" + to_handle.get_source_name(), 100, hp, &objects.get_player());
                    objects += to_make;
                }
                break;
            case EventType::Obstacle_Make:
                {
                    Unit to_make = Unit(to_handle.get_x(), to_handle.get_y(), 'O', to_handle.get_source_name(), 0, 999);
                    objects += to_make;
                }
                break;
            case EventType::Trap_Make_Left:
                {
                    Trap to_make = Trap(0, 0, '>', "enemy_" + to_handle.get_source_name(), 200, 1, &objects.get_player(), true);
                    objects += to_make;
                }
                break;
            case EventType::Trap_Make_Right:
                {
                    Trap to_make = Trap(0, active_area_cols, '<', "enemy_" + to_handle.get_source_name(), 200, 1, &objects.get_player(), false);
                    objects += to_make;
                }
                break;
            case EventType::Sweeper_Make:
                {
                    if(to_handle.get_source_name().find("super") != std::string::npos) hp = 3;
                    else hp = 1;
                    Sweeper to_make = Sweeper(0, to_handle.get_y(), 'Y', "enemy_" + to_handle.get_source_name(), 200, hp, active_area_cols);
                    objects += to_make;
                }
                break;
            case EventType::Player_Fire:
                objects.fire("player");
                break;
            case EventType::Player_Move_Up:
                objects.move_up("player");
                break;
            case EventType::Player_Move_Down:
                objects.move_down("player");
                break;
            case EventType::Player_Move_Left:
                objects.move_left("player");
                break;
            case EventType::Player_Move_Right:
                objects.move_right("player");
                break;
            case EventType::Update_State:
                objects.update();
                break;
            case EventType::Game_End:
                game_ended = true;
                stop();
                break;
            case EventType::Game_Pause:
                paused = paused ? false : true;
                break;
        } 
    }
    catch(std::out_of_range& e) // Out_of_range may be called from get_player(). If so, just end the game, since the player does not exist.
    {
        game_ended = true;
    }
}

void Worker::init()
{
    std::thread worker_thread([&](){
        while(!game_ended)
        {
            {
                std::unique_lock<std::mutex> queue_guard(event_lock); // Block thread until the event queue is not empty
                work_con.wait(queue_guard, [this](){ return !event_queue.empty(); });
                Event to_do = this->pull(); // Get event from queue, then handle
                queue_guard.unlock();
                handle(to_do);
            }
            if(game_ended) 
            {
                break;
            }
        }
    });
    worker_thread.detach(); // Run concurrent
}

Event Worker::pull()
{
    std::lock_guard<std::mutex> queue_guard(Worker::queue_lock); // Lock event queue before pull (prevents UB)
    if(!event_queue.empty()) 
    {
        Event to_pull = event_queue.front();
        event_queue.pop();
        return to_pull;
    } 
    else
    {
        throw new std::exception; // pull should never be called if the queue is empty. If it is, something is seriously wrong
    }
}

Worker operator+=(Worker& w, const Event e)
{
    {
        std::lock_guard<std::mutex> queue_guard(Worker::queue_lock); // Lock event queue before pushing to it
        Worker::event_queue.push(e);
    }
    Worker::work_con.notify_one(); // Notify worker's con_var
    return w;
}

// Clears event queue and objects
void Worker::stop()
{
    for(int i = 0; i < event_queue.size(); i++)
        event_queue.pop();
    objects.clear();
}

// Returns true if the Worker is in a state in which it can recieve events
bool Worker::can_push()
{
    return !game_ended && player_made && !paused;
}

// Returns true if the worker considers the game to be over
bool Worker::ended()
{
    return game_ended;
}

