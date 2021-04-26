#include "Worker.h"
#include <ncurses.h>
#ifndef KEY_LISTENER_H
#define KEY_LISTENER_H
class KeyListener
{
private:
    Worker& to_push;
    WINDOW* key_win;
    bool end;
    bool repeat;
public:
    KeyListener(Worker&);
    bool want_repeat();
    void init();
};
#endif