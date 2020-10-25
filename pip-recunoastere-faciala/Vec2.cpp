#pragma once
#include "Vec2.hpp"

Vec2::Vec2(int x, int y) : x(x), y(y)
{}

Vec2::Vec2(const Vec2& other) : x(other.x), y(other.y)
{}

Vec2& Vec2::operator=(const Vec2& other)
{
	x = other.x;
	y = other.y;

	return *this;
}

Vec2 Vec2::operator+(const Vec2& other)
{
	return Vec2(x + other.x, y + other.y);
}
Vec2 Vec2::operator-(const Vec2& other)
{
	return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator+=(const Vec2& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Vec2 Vec2::operator-=(const Vec2& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Vec2 Vec2::operator*(int scale)
{
	return Vec2(x * scale, y * scale);
}

Vec2 Vec2::operator*(float scale)
{
	return Vec2((int)(x * scale), (int)(y * scale));
}
