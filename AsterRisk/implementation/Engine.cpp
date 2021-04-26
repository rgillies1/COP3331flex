#include "../headers/Engine.h"
#include "../headers/Event.h"
#include <thread>
#include <random>
#include <ctime>

std::vector<Event> Engine::line;

Engine::Engine(Worker& to_push, int x, int y, int refresh_rate) : to_push(to_push), x(x-2), y(y - (3/3)), refresh_rate(refresh_rate)
{
    obstacle_instances = 0;
    follower_instances = 0;
    trap_instances = 0;
    random_seed = time(0) + std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // Arbitrarily large number for random seed
    srand(random_seed);
    cycles_since_update = 0;
    cycles_since_start = 0;
}

void Engine::init()
{
    std::thread engine_thread([this](){
        generate_line(); // Intial line
        while(!to_push.ended())
        {
            if(!to_push.can_push()) continue; // Halt until the Worker allows for input (the user presses space)
            if(cycles_since_start >= MAX_CYCLES) // At max cyckes, end the game immediately
            {
                to_push += Event(EventType::Game_End);
                break;
            }
            if(cycles_since_update == 30 && cycles_since_start <= MAX_CYCLES - (30*refresh_rate)) // Stop pushing new units for the last few cycles
            {
                push_line();
                generate_line();
                cycles_since_update = 0;
            }
            else cycles_since_update++;
            to_push += Event(EventType::Update_State);
            cycles_since_start++;
            std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate));
        }
    });
    engine_thread.detach();
}

void Engine::generate_line()
{
    int rand_type = rand() % 10; // Some value 0-9
    int super_modulo;
    int super;
    std::string name;

    // The further into the game you go, the more likely you are to spawn red enemies
    if(cycles_since_start >= MAX_CYCLES - (MAX_CYCLES / 4)) super_modulo = 5;
    else if(cycles_since_start >= MAX_CYCLES / 2) super_modulo = 10;
    else super_modulo = 20;
    switch(rand_type)
    {
        case 0: // Random line of obstacles with no units
        case 1:
        case 2:
        case 3:
        case 4: // Line of obstacles and barriers
            for(int i = 0; i < y; i++)
            {
                int yorn = rand() % 10;
                if(yorn == 0 || yorn == 1 || yorn == 2)
                {
                    line.push_back(Event("obstacle_" + std::to_string(obstacle_instances++), EventType::Obstacle_Make, 0, i));
                }
            }
            break;
        case 5: // Line consisting of only a single follower
            super = rand() % super_modulo;
            if(!super) name = "super_follower";
            else name = "follower_";
            line.push_back(Event(name + std::to_string(follower_instances++), EventType::Follower_Make, 0, rand() % y));
            break;
        case 6: // Line consisting of 2 traps
        case 7:
            line.push_back(Event("trap_left_" + std::to_string(trap_instances++), EventType::Trap_Make_Left));
            line.push_back(Event("trap_right_" + std::to_string(trap_instances++), EventType::Trap_Make_Right));
            break;
            break;
        case 8: 
        case 9: // Line of only a sweeper
            super = rand() % super_modulo;
            if(!super) name = "super_sweeper";
            else name = "sweeper_";
            line.push_back(Event(name + std::to_string(sweeper_instances++), EventType::Sweeper_Make, 0, rand() % y));
            break;
    }
    if(line.empty()) generate_line();
}

// Push events to worker, clear line
void Engine::push_line()
{
    for(Event e : line) to_push += e;
    line.clear();
}