#pragma once
#include "Entity.h"

enum class Shooter
{
	Invader, Player
};

struct BulletColor
{
	unsigned __int8 r, g, b;
};

class Bullet : public Entity
{
public:
	Bullet(float x, float y, int width, int height, Shooter shooter, BulletColor c);
	Shooter GetOwner();
	BulletColor Color;
private:
	Shooter Owner;
};

