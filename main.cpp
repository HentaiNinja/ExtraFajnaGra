#ifdef _WIN32
//#include <vld.h>
#else
//g++ `pkg-config --cflags sdl2` main.cpp GameWindow.cpp GameObject.cpp `pkg-config --libs sdl2` -std=c++11
#endif

#include "GameWindow.h"

int main(int argc, char **argv) {	

	GameWindow gw;
	gw.show_start_game_window();
	return 0;
}