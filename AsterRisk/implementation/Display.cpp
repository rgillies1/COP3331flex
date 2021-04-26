#include "../headers/Display.h"
#include "../headers/ActiveObjects.h"
#include "../headers/Units/Unit.h"
#include <ncurses.h>
#include <thread>
#include <algorithm>
#define PLAYER_PAIR 1
#define OVERLAY_PAIR 2
#define ENEMY_PAIR 3
#define SUPER_PAIR 4

WINDOW* Display::overlay;
WINDOW* Display::active_area;
int Display::stat_row;
int Display::stat_col;
int Display::displayed_score;
int Display::displayed_hp;

Display::Display(ActiveObjects& objects, int rows, int cols, std::vector<int> scores)
    : objects(objects)
{
	displayed_score = -1;
	displayed_hp = -1;
	total_rows = rows;
	total_cols = cols;
	stat_row = total_rows - (5 * total_rows/6);
	stat_col = (total_cols - (total_cols / 3)) + 1;
	active_area_rows = total_rows - 2;
	active_area_cols = (total_cols - (total_cols/3)) - 1;
	high_scores = scores;
	has_begun = false;
    initscr(); // Start ncurses
	if(has_colors() == TRUE) // If the terminal supports colors, define color pairs
	{
		start_color();
		init_pair(PLAYER_PAIR, COLOR_GREEN, COLOR_BLACK);
		init_pair(OVERLAY_PAIR, COLOR_WHITE, COLOR_RED);
		init_pair(ENEMY_PAIR, COLOR_YELLOW, COLOR_BLACK);
		init_pair(SUPER_PAIR, COLOR_RED, COLOR_BLACK);
	}
	noecho(); // Don't echo key input
    clear(); // Clear current screen
    curs_set(0); // Set the cursor to position 0
	overlay = newwin(total_rows, total_cols, 0,0); // New window to facilitate writing to screen
	active_area = newwin(active_area_rows, active_area_cols, 1, 1); // Window to write to playable area
	for(int row = 0; row < total_rows; row++) // Generate display lines
	{
		std::vector<int> this_row;
		for(int col = 0; col < total_cols; col++)
		{
			if(row == 0 && col == 0) this_row.push_back(ACS_ULCORNER);
			else if(row == 0 && col == total_cols-1) this_row.push_back(ACS_URCORNER);
			else if (row == 0 && col == (total_cols - (total_cols/3))) this_row.push_back(ACS_TTEE);
			else if (row == total_rows-1 && col == (total_cols - (total_cols/3))) this_row.push_back(ACS_BTEE);
			else if(row == total_rows-1 && col == 0) this_row.push_back(ACS_LLCORNER);
			else if(row == total_rows-1 && col == total_cols-1) this_row.push_back(ACS_LRCORNER);
			else if(row == 0 || row == total_rows-1) this_row.push_back(ACS_HLINE);
			else if(col == 0 || col == total_cols-1) this_row.push_back(ACS_VLINE);
			else if (col == total_cols - (total_cols/3)) this_row.push_back(ACS_VLINE);
			else this_row.push_back(' ');
		}
		GUI.push_back(this_row);
	}
}

// Generates the GUI to be displayed
void Display::paint_screen()
{
	if(has_colors() == TRUE) wattron(overlay, COLOR_PAIR(OVERLAY_PAIR));
	wbkgd(overlay, COLOR_PAIR(OVERLAY_PAIR));
	// Write lines
	for(int row = 0; row < total_rows; row++)
	{
		for(int col = 0; col < total_cols; col++)
		{
			mvwaddch(overlay, row, col, GUI.at(row).at(col));
		}
	}
	// Write default stats
	mvwaddstr(overlay, stat_row, stat_col, "SCORE: ");
	mvwaddstr(overlay, stat_row + 1, stat_col, "LIVES: ");
	// Write high scores
	mvwaddstr(overlay, stat_row + 4, stat_col, "HISTORY:");
	int score_row = stat_row + 5;
	for(int i = 0; i < high_scores.size(); i++)
	{
		mvwaddstr(overlay, score_row++, stat_col, std::to_string(high_scores[i]).c_str());
	}
	if(high_scores.size() < 5)
	{
		for(int i = 0; i < 5 - high_scores.size(); i++)
		{
			mvwaddstr(overlay, score_row++, stat_col, "-----");
		}
	}
	// Write controls
	mvwaddstr(overlay, score_row + 4, stat_col, "CONTROLS:");
	mvwaddstr(overlay, score_row + 5, stat_col, "WASD or");
	mvwaddstr(overlay, score_row + 6, stat_col, "ARROW KEYS to move");
	mvwaddstr(overlay, score_row + 7, stat_col, "F or K to fire");
	mvwaddstr(overlay, score_row + 8, stat_col, "SPACE BAR ");
	mvwaddstr(overlay, score_row + 9, stat_col, "to toggle pause");
	mvwaddstr(overlay, score_row + 10, stat_col, "Q to quit");
	if(has_colors() == TRUE) wattroff(overlay, COLOR_PAIR(OVERLAY_PAIR));

	wrefresh(overlay);
}
// Shows the start screen
void Display::show_start()
{
	int instruction_row = active_area_rows - (3 *active_area_rows/4) - 2;
	int instruction_col = active_area_cols/8;
	mvwaddstr(overlay, instruction_row, instruction_col, "WELCOME TO ASTERISK!");
	mvwaddstr(overlay, instruction_row + 2, instruction_col, "HOW TO PLAY:");
	mvwaddstr(overlay, instruction_row + 4, instruction_col/2, "Win by making it through");
	mvwaddstr(overlay, instruction_row + 5, instruction_col/2, "he asteroid field!");
	mvwaddstr(overlay, instruction_row + 6, instruction_col/2, "Score points by hitting enemies!");
	mvwaddstr(overlay, instruction_row + 8, instruction_col/2, "ENEMY TYPES:");
	mvwaddstr(overlay, instruction_row + 10, instruction_col/2, "O - Asteroids! Don't hit these!");
	mvwaddstr(overlay, instruction_row + 12, instruction_col/2, "V - Followers! They track you down!");
	mvwaddstr(overlay, instruction_row + 14, instruction_col/2, "Y - Sweepers! These move");
	mvwaddstr(overlay, instruction_row + 15, instruction_col/2, "back and forth!");
	mvwaddstr(overlay, instruction_row + 16, instruction_col/2, ">< - Traps! These will");
	mvwaddstr(overlay, instruction_row + 17, instruction_col/2, "try to ram you!");
	mvwaddstr(overlay, instruction_row + 18, instruction_col/2, "YELLOW enemies are normal.");
	mvwaddstr(overlay, instruction_row + 20, instruction_col/2, "RED enemies are stronger,");
	mvwaddstr(overlay, instruction_row + 21, instruction_col/2, "and take more hits.");
	mvwaddstr(overlay, instruction_row + 22, instruction_col/2, "(Press SPACE to begin)");
	wrefresh(overlay);
}

// Waits for objects to not be empty.
// If not empty, attempts to acquire the lock, then passes write to objects' iterate_and_do()
void Display::show()
{
	paint_screen();
	show_start();
	std::thread display_thread([&](){
		while(true)
		{
			if(!objects.is_empty())
			{
				has_begun = true;
				werase(active_area);
				objects.iterate_and_do(write);
				//update_stats();
				wrefresh(active_area);
			}
			else if(objects.is_empty() && has_begun) 
			{
				stop();
				break;
			}
		}
	});
	display_thread.detach();
}

// Gracefully exits ncurses, returns the current score
int Display::exit_screen()
{
	clear();
	endwin();
	return displayed_score;
}

// Writes the health and score of the passed unit to the screen
void Display::update_stats(Unit& p)
{
	int score_row = stat_row;
	int score_col = stat_col + 7;
	int life_row = stat_row + 1;
	int life_col = stat_col + 7;
	WINDOW* value_area = derwin(overlay, 2, 7, score_row, score_col);
	werase(value_area);
	std::string life_string = "";
	for(int i = 0; i < p.get_health(); i++)
		life_string.push_back('+');
	std::string score_string = "0000";
	mvwaddstr(overlay, score_row, score_col, std::to_string(p.get_score()).c_str());
	mvwaddstr(overlay, life_row, life_col, life_string.c_str());
	wrefresh(value_area);
	if(p.get_health() == 0 && p.get_score() == -1) return;
	displayed_hp = p.get_health();
	displayed_score = p.get_score();
}

// Halts the game and displays the game over screen
void Display::stop()
{
	paint_screen();
	std::string game_over_str = "GAME OVER!";
	std::string score_str = "SCORE: " + std::to_string(displayed_score);
	std::string restart_str = "Press r to restart";
	std::string quit_str = "Press q to quit";
	mvwaddstr(overlay, active_area_rows/2, (active_area_cols/2) - (game_over_str.size()/2), game_over_str.c_str());
	mvwaddstr(overlay, active_area_rows/2 + 1, (active_area_cols/2) - (score_str.size()/2), score_str.c_str());
	mvwaddstr(overlay, active_area_rows/2 + 2, (active_area_cols/2) - (restart_str.size()/2), restart_str.c_str());
	mvwaddstr(overlay, active_area_rows/2 + 3, (active_area_cols/2) - (quit_str.size()/2), quit_str.c_str());
	wrefresh(active_area);
	wrefresh(overlay);
}

// Static so that it can be passed to ActiveObjects' iterate_and_do without a circular include
// Writes the passed unit to the screen
void Display::write(Unit& i)
{
	int color;
	if(i.get_name() == "player") update_stats(i);
	if(has_colors() == TRUE) {
		if(i.get_name().find("player") != std::string::npos) color = PLAYER_PAIR;
		else if(i.get_name().find("super") != std::string::npos) color = SUPER_PAIR;
		else if(i.get_name().find("enemy") != std::string::npos) color = ENEMY_PAIR;
		else color = 0;
		wattron(active_area, COLOR_PAIR(color));
		mvwaddch(active_area, i.get_x(), i.get_y(), i.get_representation());
		wattroff(active_area, COLOR_PAIR(color));
	}
	else mvwaddch(active_area, i.get_x(), i.get_y(), i.get_representation());
}