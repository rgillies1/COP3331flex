#include <vector>
#include "Display.h"
#include "Worker.h"
#define MAX_CYCLES 4500
#ifndef ENGINE_H
#define ENGINE_H
class Engine
{
private:
    Worker& to_push;
    const int x;
    const int y;
    const int refresh_rate;
    static std::vector<Event> line;
    int obstacle_instances;
    int follower_instances;
    int trap_instances;
    int sweeper_instances;
    void generate_line();
    void push_line();
    int cycles_since_update; // # of times the engine has pushed updates since the last time it added new units
    int cycles_since_start; // # of updates pushed since the game started
    unsigned int random_seed;
public:
    Engine(Worker& to_push, int, int, int);
    void init();
};
#endif