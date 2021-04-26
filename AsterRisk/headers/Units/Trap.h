#include "Unit.h"
#ifndef TRAP_H
#define TRAP_H
class Trap : public Unit
{
private:
    Unit* target;
    bool target_had_same_x;
    bool faces_right;
    int triggered_x;
public:
    Trap(int, int, char, std::string, int, int, Unit*, bool);
    void do_action() override;
};
#endif