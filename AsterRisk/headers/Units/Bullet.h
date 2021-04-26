#include "Unit.h"
#ifndef BULLET_H
#define BULLET_H
class Bullet : public Unit
{
private:
    int x;
    int y;
    char representation;
    std::string name;
    std::string origin_name;
    bool moves_up;
public:
    Bullet(int, int, char, std::string, std::string, bool);
    std::string get_origin_name();
    void do_action() override;
    void die() override;
};
#endif