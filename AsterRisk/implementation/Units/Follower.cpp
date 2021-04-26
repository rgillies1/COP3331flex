#include "../../headers/Units/Follower.h"

Follower::Follower(int x, int y, char rep, std::string name, int score, int max_health, Unit* to_follow)
: Unit(x, y, rep, name, score, max_health)
{
    target = to_follow;
}

// Attempt to match the x of the target
// Moves in the direction needed to make this Follower and the target's x values equal
void Follower::do_action()
{
    int y_to_follow = target->get_y();

    if(y_to_follow > this->get_y()) {
        this->set_y(this->get_y() + 1);
        fire_cycles++;
    }
    else if(y_to_follow < this->get_y()) {
        this->set_y(this->get_y() - 1);
        fire_cycles++;
    }
    else if(y_to_follow == this->get_y()) {
        if(fire_cycles >= 10) 
        {
            this->set_fire(true);
            fire_cycles = 0;
        }
        else fire_cycles++;
    }
}
