#include <ncurses.h>
#include <vector>
#include "./Units/Unit.h"
#include "ActiveObjects.h"
#ifndef DISPLAY_H
#define DISPLAY_H
class Display
{
private:
    ActiveObjects& objects;
    static WINDOW* overlay;
    static WINDOW* active_area;
    int total_rows;
    int total_cols;
    static int stat_row; // Starting x of stat area
    static int stat_col; // Starting y of stat area
    int active_area_rows;
    int active_area_cols;
    static int displayed_score;
    static int displayed_hp;
    std::vector<std::vector<int>> GUI; // Stores lines of display
    std::vector<int> high_scores;
    bool has_begun;
    void stop();
public:
    Display(ActiveObjects&, int, int, std::vector<int>);
    void paint_screen();
    void show_start();
    void show();
    int exit_screen();
    static void update_stats(Unit&);
    static void write(Unit&);
};

#endif