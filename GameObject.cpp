#include "GameObject.h"

GameObject::GameObject(int width, int height, Physics physics, SDL_Texture* texture)
{
	this->width = width;
	this->height = height;
	this->physics = physics;
	this->texture = texture;
}

double GameObject::i()
{
	return physics.position[0];
}

double GameObject::j()
{
	return physics.position[1];
}

void GameObject::i(double i)
{
	physics.position[0] = i;
}

void GameObject::j(double j)
{
	physics.position[1] = j;
}

GameObject::~GameObject()
{
}
