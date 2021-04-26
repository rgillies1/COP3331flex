#include "../../headers/Units/Trap.h"

Trap::Trap(int x, int y, char rep, std::string name, int score, int max_health, Unit* target, bool faces_right)
: Unit(x, y, rep, name, score, max_health)
{
    this->target = target;
    this->faces_right = faces_right;
    target_had_same_x = false;
}

// Idle until the target has the same x value. Then constantly move left or right, depending on the value of faces_right
void Trap::do_action()
{
    int target_x = target->get_x();
    if(this->get_x() == target_x) 
    {
        target_had_same_x = true;
    }
    if(target_had_same_x && faces_right) 
    {
        this->set_y(this->get_y() + 1);
    }
    else if(target_had_same_x && !faces_right)
    {
        this->set_y(this->get_y() - 1);
    }
}

