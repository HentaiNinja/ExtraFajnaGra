#include "KeyboardHandler.h"



KeyboardHandler::KeyboardHandler()
{
}


KeyboardHandler::~KeyboardHandler()
{
}

void KeyboardHandler::handle(SDL_Event ev)
{
	keyStates[ev.key.keysym.sym] = ev.key.state;
}

bool KeyboardHandler::is_pressed(int keyCode)
{
	std::map<int, bool>::iterator a = keyStates.find(keyCode);
	if (a == keyStates.end())
		return false;
	else
		return a->second;
}

void KeyboardHandler::clear_events()
{
	keyStates.clear();
}
