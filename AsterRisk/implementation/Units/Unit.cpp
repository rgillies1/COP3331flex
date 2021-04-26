#include "../../headers/Units/Unit.h"
#include <string>
Unit::Unit(int x, int y, int representation, std::string name, int score, int max_health)
{
    current_x = x;
    current_y = y;
    this->representation = representation;
    this->name = name;
    this->score = score;
    this->current_health = max_health;
    dead = false;
    fire_cycles = 0;
    firing = false;
    death_cycles = 0;
}

void Unit::set_x(int x)
{
    current_x = x;
}

void Unit::set_y(int y)
{
    current_y = y;
}

int Unit::get_x() const
{
    return current_x;
}

int Unit::get_y() const
{
    return current_y;
}

char Unit::get_representation() const
{
    return representation;
}

std::string Unit::get_name() const
{
    return name;
}

// Lower health by 1, then check if unit is dead
void Unit::hurt()
{
    current_health--;
    if(current_health == 0) die();
}

// Default behavior is to set unit's representation to X upon death
void Unit::die()
{
    dead = true;
    set_representation('X');
}

bool Unit::is_dead()
{
    return this->dead;
}

// Default behavior (shared by player and obstacles) is to do nothing
void Unit::do_action()
{
    return;
}

int Unit::get_score()
{
    return this->score;
}
void Unit::set_score(int score)
{
    this->score = score;
}
int Unit::get_health()
{
    return this->current_health;
}
int Unit::get_death_cycles()
{
    return this->death_cycles;
}
void Unit::do_death_cycle()
{
    this->death_cycles++;
}

bool Unit::is_firing()
{
    return firing;
}

void Unit::set_fire(bool firing)
{
    this->firing = firing;
}

int Unit::get_fire_cycles() const
{
    return this->fire_cycles;
}

void Unit::set_representation(char to_set)
{
    representation = to_set;
}