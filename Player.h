#pragma once
#include "Entity.h"

class Player : public Entity
{
public:
	Player(float x, float y, int width, int height);

	void TakeDamage(int damage);
	int GetLives();

	bool canShoot;
private:
	int Lives;
};

