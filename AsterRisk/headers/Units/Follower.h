#include "Unit.h"
#ifndef FOLLOWER_H
#define FOLLOWER_H
class Follower : public Unit
{
private:
    Unit* target;
public:
    Follower(int, int, char, std::string, int, int, Unit*);
    void do_action() override;
};
#endif