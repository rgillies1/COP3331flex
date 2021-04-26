#include "./Units/Unit.h"
#include <memory>
#ifndef EVENT_H
#define EVENT_H
enum class EventType
{
    Player_Make,
    Player_Fire,
    Player_Move_Up,
    Player_Move_Down,
    Player_Move_Left,
    Player_Move_Right,
    Obstacle_Make,
    Follower_Make,
    Trap_Make_Left,
    Trap_Make_Right,
    Sweeper_Make,
    Update_State,
    Game_End,
    Game_Pause
};
class Event 
{
private:
    std::string subject_name = "";
    EventType type;
    int x = -1;
    int y = -1;
public:
    Event(std::string, EventType, int, int);
    Event(std::string, EventType);
    Event(EventType);
    std::string get_source_name();
    EventType get_event_type();
    int get_x();
    int get_y();
};
#endif