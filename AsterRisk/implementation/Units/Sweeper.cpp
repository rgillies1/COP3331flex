#include "../../headers/Units/Sweeper.h"

Sweeper::Sweeper(int x, int y, char rep, std::string name, int score, int max_health, int max_y)
: Unit(x, y, rep, name, score, max_health)
{
    this->max_y = max_y;
    moving_left = false;
    cycles_since_move = 0;
}

// Move back and forth, fire nearly constantly
void Sweeper::do_action()
{
    if(this->get_fire_cycles() >= 5) 
    {
        set_fire(true);
        fire_cycles = 0;
    }
    else fire_cycles++;
    if(cycles_since_move >= 1)
    {
        if(this->get_y() == 0) moving_left = false;
        else if(this->get_y() == max_y) moving_left = true;

        if(moving_left) this->set_y(this->get_y() - 1);
        else this->set_y(this->get_y() + 1);
        cycles_since_move = 0;
    }
    else cycles_since_move++;

}