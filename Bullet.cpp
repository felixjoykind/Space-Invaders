#include "Bullet.h"

Bullet::Bullet(float x, float y, int width, int height, Shooter shooter, BulletColor c)
	:Entity(x, y, width, height) {
	Owner = shooter;
	Color = c;
}

Shooter Bullet::GetOwner() { return Owner; }