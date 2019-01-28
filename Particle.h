#pragma once
#include "GameObject.h"

class Particle: public GameObject
{
public:
	int r;
	int g;
	int b;
	int a;
	int deathtime;

	Particle(int width, int height, Physics physics, SDL_Texture* texture, int r, int g, int b, int a, int deathtime);
	Particle(GameObject go,	int r, int g, int b, int a, int deathtime);
	~Particle();
};

