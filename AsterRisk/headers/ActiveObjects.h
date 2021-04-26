#include <map>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <memory>
#include "./Units/Unit.h"
#include "./Units/Bullet.h"
#include "./Units/Follower.h"
#include "./Units/Sweeper.h"
#include "./Units/Trap.h"
#ifndef ACTIVE_OBJECTS_H
#define ACTIVE_OBJECTS_H
class ActiveObjects
{
private:
    static std::unordered_map<std::string, std::shared_ptr<Unit>> active;
    static std::mutex active_lock;
    int updates_since_shift; // # of updates processed since the last call to shift_down
    int num_bullets_lifetime; // # of bullets ever fired among all units
    int active_area_rows;
    int active_area_cols;
    void shift_down();
    void check_collide(Unit&);
    void process_death(Unit&);
public:
    ActiveObjects(int, int);
    ActiveObjects& operator+=(Unit&);
    ActiveObjects& operator+=(Follower&);
    ActiveObjects& operator+=(Trap&);
    ActiveObjects& operator+=(Sweeper&);
    void update();
    bool is_empty();
    Unit& get_player();
    void fire(std::string);
    void move_up(std::string);
    void move_down(std::string);
    void move_left(std::string);
    void move_right(std::string);
    void clear();
    bool has_player();
    void iterate_and_do(void f(Unit&));
};
#endif