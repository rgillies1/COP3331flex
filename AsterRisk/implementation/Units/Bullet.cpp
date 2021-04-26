#include "../../headers/Units/Bullet.h"

Bullet::Bullet(int x, 
               int y, 
               char representation, 
               std::string name, 
               std::string origin_name,
               bool moves_up)
: Unit(x, y, representation, name, 0, 1)
{
    this->origin_name = origin_name;
    this->moves_up = moves_up;
    this->death_cycles = 0;
}

// Move either up or down once, depending on moves_up
void Bullet::do_action()
{
    if(moves_up) this->set_x(this->get_x() - 1);
    else this->set_x(this->get_x() + 1);
}

// Name of unit this bullet came from
std::string Bullet::get_origin_name()
{
    return origin_name;
}

// Does not display an X on death, dies much faster
void Bullet::die()
{
    dead = true;
    death_cycles = 10;
}

