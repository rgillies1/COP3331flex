#include <string>
#ifndef UNIT_H
#define UNIT_H
class Unit
{    
private:
    int current_x;
    int current_y;
    int representation;
    std::string name;
    bool firing;
    int score;
    int current_health;
    void set_representation(char);
protected:
    bool dead;
    int fire_cycles;
    int death_cycles;
public:
    Unit(int, int, int, std::string, int, int);
    void set_x(int);
    void set_y(int);
    int get_x() const;
    int get_y() const;
    char get_representation() const;
    int get_color() const;
    std::string get_name() const;
    bool is_dead();
    void hurt();
    virtual void die();
    virtual void do_action();
    void destroy();
    int get_score();
    void set_score(int);
    int get_health();
    int get_death_cycles();
    void do_death_cycle();
    bool is_firing();
    void set_fire(bool);
    int get_fire_cycles() const;
};
#endif