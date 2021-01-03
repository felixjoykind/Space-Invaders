#include "Entity.h"

Entity::Entity(float x, float y, int width, int height)
{
	X = x;
	Y = y;
    Width = width;
    Height = height;
}

void Entity::Move(float offset_x, float offset_y)
{
	X += offset_x;
	Y += offset_y;
}

bool Entity::Intersect(Entity other)
{
    if ((this->GetX() > other.GetX() + other.GetWidth()) || (this->GetX() + this->GetWidth() < other.GetX())
        || (this->GetY() > other.GetY() + other.GetHeight()) || (this->GetY() + this->GetWidth() < other.GetY()))
    {
        return false;
    }
    else
        return true;
}

bool Entity::operator== (const Entity& other)
{
    return other.X == this->X && other.Y == other.Y;
}

float Entity::GetX() { return X; }
float Entity::GetY() { return Y; }

float Entity::GetWidth() { return Width; }
float Entity::GetHeight() { return Height; }