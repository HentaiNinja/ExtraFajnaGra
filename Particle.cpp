#include "Particle.h"



Particle::Particle(GameObject go, int r, int g, int b, int a, int deathtime) : GameObject(go.width, go.height, go.physics, go.texture)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
	this->deathtime = deathtime;
}

Particle::Particle(int width, int height, Physics physics, SDL_Texture* texture, int r, int g, int b, int a, int deathtime):
	GameObject(width, height, physics, texture)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
	this->deathtime = deathtime;
}



Particle::~Particle()
{
}
