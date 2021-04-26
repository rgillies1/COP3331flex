#include "Unit.h"
#ifndef SWEEPER_H
#define SWEEPER_H
class Sweeper : public Unit
{
private:
    int max_y;
    bool moving_left;
    int cycles_since_move;
public:
    Sweeper(int, int, char, std::string, int, int, int);
    void do_action() override;
};




#endif