#pragma once
#include <map>
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif // _WIN32


class KeyboardHandler
{
private:
	std::map<int, bool> keyStates;
public:
	KeyboardHandler();
	~KeyboardHandler();
	void handle(SDL_Event event);
	bool is_pressed(int keyCode);
	void clear_events();
};

