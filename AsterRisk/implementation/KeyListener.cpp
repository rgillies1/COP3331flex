#include "../headers/KeyListener.h"
#include "../headers/Event.h"

KeyListener::KeyListener(Worker& to_push) : to_push(to_push)
{
	key_win = newwin(0,0,0,0); // Blank ncurses window to capture key input
	raw(); // Capture raw key input (does not need to be typed and then entered)
	keypad(key_win, TRUE); // Capture arrow key input
	wrefresh(key_win);
	end = false;
}

bool KeyListener::want_repeat()
{
	return repeat;
}

void KeyListener::init()
{
	std::thread listener_thread([&](){
		bool started = false;
		int input;
		input = wgetch(key_win);
		while (!end)
		{
			switch(input)
			{
			case ' ': // Start or toggle pause
				if(end) break;
				if(started) to_push += Event(EventType::Game_Pause);
				else 
				{
					to_push += Event(EventType::Player_Make);
					started = true;
				}
				break;
			case KEY_UP: // Move up
			case 'w':
				if(end) break;
				to_push += Event(EventType::Player_Move_Up);
				break;
			case KEY_DOWN: // Move down
			case 's':
				if(end) break;
				to_push += Event(EventType::Player_Move_Down);
				break;
			case KEY_LEFT: // Move left
			case 'a':
				if(end) break;
				to_push += Event(EventType::Player_Move_Left);
				break;
			case KEY_RIGHT: // Move right
			case 'd':
				if(end) break;
				to_push += Event(EventType::Player_Move_Right);
				break;
			case 'f': // Fire
			case 'k':
				if(end) break;
				to_push += Event(EventType::Player_Fire);
				break;
			case 'q': // Quit
				if(to_push.ended()) 
				{
					end = true;
					repeat = false;
				}
				else to_push += Event(EventType::Game_End);
				break;
			case 'r': // Restart
				if(to_push.ended()) 
				{
					end = true;
					repeat = true;
				}
				break;
			default: // Do nothing with other keystrokes
				break;
			}
			if(end) break;
			input = wgetch(key_win);
		}
	});
	if(listener_thread.joinable()) listener_thread.join(); 
	else throw new std::exception; // If this thread can't be joined to main, the program can't be run, so don't bother
}