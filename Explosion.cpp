#include "Explosion.h"

Explosion::Explosion(float x, float y, float width, float height)
	:Entity(x, y, width, height) {
	elapsed = 0.0f;
}