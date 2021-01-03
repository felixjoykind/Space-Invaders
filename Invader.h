#pragma once
#include "Entity.h"

class Invader : public Entity
{
public:
	Invader(float x, float y, int width, int height);

	bool isAlive;
};
