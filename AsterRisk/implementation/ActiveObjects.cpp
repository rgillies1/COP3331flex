#include "../headers/ActiveObjects.h"
#include <exception>
#include <algorithm>
#include <ncurses.h>
#include <memory>
#include <new>

std::unordered_map<std::string, std::shared_ptr<Unit>> ActiveObjects::active;
std::mutex ActiveObjects::active_lock;

ActiveObjects::ActiveObjects(int rows, int cols)
{
    num_bullets_lifetime = 0;
    updates_since_shift = 0;
    active_area_rows = rows - 2;
    active_area_cols = (cols - cols/3) - 2;
}

// Operator overloads are the easiest way to get around slicing when calling make_shared
ActiveObjects& ActiveObjects::operator+=(Unit& to_add)
{
    std::lock_guard<std::mutex> update_lock(active_lock);
    std::string name = to_add.get_name();
    active.insert(std::pair<std::string,std::shared_ptr<Unit>>(name, std::make_shared<Unit>(to_add)));
    return *this;
}

ActiveObjects& ActiveObjects::operator+=(Follower& to_add)
{
    std::lock_guard<std::mutex> update_lock(active_lock);
    std::string name = to_add.get_name();
    active.insert(std::pair<std::string,std::shared_ptr<Unit>>(name, std::make_shared<Follower>(to_add)));
    return *this;
}

ActiveObjects& ActiveObjects::operator+=(Trap& to_add)
{
    std::lock_guard<std::mutex> update_lock(active_lock);
    std::string name = to_add.get_name();
    active.insert(std::pair<std::string,std::shared_ptr<Unit>>(name, std::make_shared<Trap>(to_add)));
    return *this;
}

ActiveObjects& ActiveObjects::operator+=(Sweeper& to_add)
{
    std::lock_guard<std::mutex> update_lock(active_lock);
    std::string name = to_add.get_name();
    active.insert(std::pair<std::string,std::shared_ptr<Unit>>(name, std::make_shared<Sweeper>(to_add)));
    return *this;
}

// Updates every object in the map
void ActiveObjects::update()
{
    std::lock_guard<std::mutex> update_lock(active_lock); // Lock map
    if(active.find("player") == active.end() || active.at("player")->is_dead()) // If this function ever runs while the player is not in the map, game over
    {
        active.clear();
    }
    if(updates_since_shift == 25) // Shift all units on screen down once every 25 updates
    {
        shift_down();
        updates_since_shift = 0;
    } else updates_since_shift++;
    for(auto it = active.begin(); it != active.end();) // Main update function - iterate over map
    {
        auto u = it; // Make seperate iterator so removing it from the map does not invalidate loop control
        it++;
        process_death(*u->second); // Check if this unit is dead
        if(u->second->is_dead() || active.find(u->second->get_name()) == active.end()) continue; // If so, move to the next
        if(u->second->get_x() >= active_area_rows) u->second->die(); // If the unit is offscreen, just kill them
        if(u->second->get_x() <= -1) u->second->die(); // Same as above
        if(u->second->get_y() <= -1) u->second->die(); // Same as above
        if(u->second->get_y() >= active_area_cols + 1) u->second->die(); // Same as above
        u->second->do_action(); // Polymorphic do function - each unit performs their assigned action
        if(u->second->get_name().find("obstacle") != std::string::npos) continue; // Don't bother to do collision detection for obstacles
        check_collide(*u->second); // Collision detection
        if(active.find("player") == active.end() || active.at("player")->is_dead()) break; // If the player dies after collision detection, game over
        process_death(*u->second);
        if(u->second->is_firing()) // Process firing projectile
        {
            fire(u->second->get_name());
            u->second->set_fire(false);
        }
    }
}

void ActiveObjects::fire(std::string from_key)
{
    bool d;
    int y;
    int x;
    char rep;
    if(active.find("player") == active.end() || active.at("player")->is_dead()) return; // If the player is dead, don't bother, since game over
    if(active.find(from_key) == active.end() || active.at(from_key)->is_dead()) return; // If the unit that wants to fire is dead, don't bother
    Unit& from = *active.at(from_key);
    std::string bullet_name = from_key + "_bullet" + std::to_string(num_bullets_lifetime++);
    if(from_key == "player")
    {
        d = true;
        y = from.get_y();
        x = from.get_x();
        rep = '^';
    } 
    else 
    {
        d = false;
        y = from.get_y();
        x = from.get_x();
        rep = '*';
    }
    Bullet to_fire = Bullet(x, y, rep, bullet_name, from.get_name(), d); // Bullet is generated on top of the unit that fires
    active.insert(std::pair<std::string, std::shared_ptr<Unit>>(bullet_name, std::make_shared<Bullet>(to_fire)));
    check_collide(to_fire); // Technically redundant
}

bool ActiveObjects::is_empty()
{
    return active.empty();
}

// Collision detection
void ActiveObjects::check_collide(Unit& to_check)
{
    if(to_check.is_dead()) return; // Don't check if dead units are colliding
    auto collide_it = std::find_if(active.begin(), active.end(), [&](std::pair<const std::string, std::shared_ptr<Unit>> &p)
    {
        // Get the first object in the map that occupies the same spot but doesn't have the same name
        return (p.first != to_check.get_name() 
                && p.second->get_x() == to_check.get_x() 
                && p.second->get_y() == to_check.get_y()
                && to_check.get_name().find(p.second->get_name()) == std::string::npos);
    });
    if(collide_it != active.end() // Continue if such an object exists
    && !collide_it->second->is_dead()) // And is not dead
    {
        // Do not collide if both originate from a player or an enemy
        if(to_check.get_name().find("enemy") != std::string::npos && collide_it->second->get_name().find("enemy") != std::string::npos) return;
        if(to_check.get_name().find("player") != std::string::npos && collide_it->second->get_name().find("player") != std::string::npos) return;
        // Otherwise, hurt both
        to_check.hurt();
        collide_it->second->hurt();
        // If the player dies afterward, game over (next update will process this)
        if(active.find("player") == active.end() || active.at("player")->is_dead()) return;
        // If the player was involed in the collision, give the player points
        if(collide_it->second->is_dead() && to_check.get_name().find("player") != std::string::npos) 
        {
            active.at("player")->set_score(active.at("player")->get_score() + collide_it->second->get_score());
        }
        if(to_check.is_dead() && collide_it->second->get_name().find("player") != std::string::npos)
        {
            active.at("player")->set_score(active.at("player")->get_score() + to_check.get_score());
        }
    }
}

// Returns the active player Unit, if it exists.
// Throws out_of_range if no such Unit exists
Unit& ActiveObjects::get_player()
{
    std::lock_guard<std::mutex> update_lock(active_lock);
    if(active.find("player") != active.end() && !active.at("player")->is_dead()) return *active.at("player");
    else throw new std::out_of_range("Player was not found");
}

// Shifts active units down once
void ActiveObjects::shift_down()
{
    for(auto it= active.begin(); it != active.end(); it++)
    {
        if(it->second->get_name() == "player") continue; // Don't move the player
        if(it->second->is_dead()) continue; // Don't move dead units
        it->second->set_x(it->second->get_x() + 1);
    }
}

// Moves the unit at the passed key up once, if it exists
// If it does not exist, do nothing
void ActiveObjects::move_up(std::string key)
{
    if(active.find(key) == active.end()) return;
    if(active.at(key)->get_x() <= 0) return;
    active.at(key)->set_x(active.at(key)->get_x() - 1);
}

void ActiveObjects::move_down(std::string key)
{
    std::lock_guard<std::mutex> update_lock(active_lock);
    if(active.find(key) == active.end()) return;
    if(active.at(key)->get_x() >= active_area_rows - 1) return;
    active.at(key)->set_x(active.at(key)->get_x() + 1);
}

void ActiveObjects::move_left(std::string key)
{
    std::lock_guard<std::mutex> update_lock(active_lock);
    if(active.find(key) == active.end()) return;
    if(active.at(key)->get_y() <= 0) return;
    active.at(key)->set_y(active.at(key)->get_y() - 1);
}

void ActiveObjects::move_right(std::string key)
{
    std::lock_guard<std::mutex> update_lock(active_lock);
    if(active.find(key) == active.end()) return;
    if(active.at(key)->get_y() >= active_area_cols) return;
    active.at(key)->set_y(active.at(key)->get_y() + 1);
}

// Locks the map, then clears it
// Free memory is handles by shared_ptrs
void ActiveObjects::clear()
{
    std::lock_guard<std::mutex> update_lock(active_lock);
    active.clear();
}

// Returns true if a Unit named player exists, false otherwise
bool ActiveObjects::has_player()
{
    if(active.find("player") != active.end() && !active.at("player")->is_dead()) return true;
    else return false;
}

// If the checks if the Unit passed is dead, and processes its death cycles if so
void ActiveObjects::process_death(Unit& u)
{
    if(u.is_dead())
    {
        if(u.get_death_cycles() < 10) 
        {
            u.do_death_cycle();
        }
        else 
        {
            active.erase(u.get_name());
        }
    }
}

// Iterates over the map and performs the passed function on each unit
// This is meant to be used with Display's write() function
void ActiveObjects::iterate_and_do(void f(Unit&))
{
    std::lock_guard<std::mutex> update_lock(active_lock);
    for(auto it = active.begin(); it != active.end(); it++)
        f(*it->second);
}