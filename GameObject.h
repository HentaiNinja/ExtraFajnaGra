#pragma once
#include "Physics.h"

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif // _WIN32
class GameObject
{
public:
	int width;
	int height;
	Physics physics;
	SDL_Texture* texture;
	GameObject(int width, int height, Physics physics, SDL_Texture* texture);
	double i();
	double j();
	void i(double i);
	void j(double j);
	~GameObject();
};

