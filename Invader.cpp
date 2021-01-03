#include "Invader.h"

Invader::Invader(float x, float y, int width, int height)
	: Entity(x, y, width, height) {
	isAlive = true;
}