#pragma once

class Entity
{
public:
	Entity(float x, float y, int width, int height);

	float GetX();
	float GetY();

	float GetWidth();
	float GetHeight();

	void Move(float offset_x, float offset_y);
	bool Intersect(Entity other);

	bool operator==(const Entity& other);
private:
	float X, Y;
	float Width, Height;
};

