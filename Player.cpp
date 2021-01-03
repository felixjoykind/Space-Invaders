#include "Player.h"

Player::Player(float x, float y, int width, int height)
	: Entity(x, y, width, height) {
	Lives = 3;
	canShoot = true;
}

void Player::TakeDamage(int damage) { Lives -= damage; }
int Player::GetLives() { return Lives; }