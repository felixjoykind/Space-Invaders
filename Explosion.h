#pragma once
#include "Entity.h"

class Explosion : public Entity
{
public:
	Explosion(float x, float y, float width, float height);
	float elapsed;
};

