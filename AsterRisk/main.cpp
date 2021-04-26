#include "./headers/KeyListener.h"
#include "./headers/Display.h"
#include "./headers/KeyListener.h"
#include "./headers/Worker.h"
#include "./headers/Engine.h"
#include "./headers/ActiveObjects.h" 
#include <iostream>
#include <ncurses.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#define NUM_ROWS 30 // The number of terminal row the game screen should span
#define NUM_COLS 60 // The number of terminal columns the game screen should span
#define SCORE_FILE_NAME "scores.txt" // The name of the file scores should be saved to
#define REFRESH_RATE 30 // The milliseconds the Engine should wait before executing a cycle
int main()
{
    bool repeat = false; // Tracks if the program should restart
    do
    {
        std::vector<int> scores;
        std::ifstream file_input_stream;
        std::ofstream file_output_stream;
        std::string temp;
        std::stringstream ss; // For input verification
        int to_push;
        // Get scores from file
        file_input_stream.open(SCORE_FILE_NAME);
        while(getline(file_input_stream, temp))
        {
            ss.str(temp);
            ss.clear();
            ss >> to_push;
            scores.push_back(to_push);
        }
        file_input_stream.close();

        std::sort(scores.begin(), scores.end(), [](int i, int j){ return i > j; }); // Sort descending
        if(scores.size() > 5) scores.resize(5); // Keep 5 highest scores

        ActiveObjects active_obj = ActiveObjects(NUM_ROWS, NUM_COLS);
        Display display = Display(active_obj, NUM_ROWS, NUM_COLS, scores);
        Worker worker = Worker(active_obj, NUM_ROWS, NUM_COLS);
        Engine engine = Engine(worker, NUM_ROWS, NUM_COLS, REFRESH_RATE);
        KeyListener key_listener = KeyListener(worker);
        
        worker.init();
        engine.init();
        display.show();
        key_listener.init();

        int final_score = display.exit_screen();
        repeat = key_listener.want_repeat();

        file_output_stream.open(SCORE_FILE_NAME, std::ofstream::app); // Open score file, append to end
        if(final_score != -1 && final_score != 0) // Only append if non-zero
            file_output_stream << final_score << std::endl;
        file_output_stream.close();
    } while (repeat);
}