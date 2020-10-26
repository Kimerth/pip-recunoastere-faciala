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

Vec2 operator*(const Vec2& vec, double scale)
{
	return Vec2(static_cast<int>(vec.x * scale), static_cast<int>(vec.y * scale));
}
Vec2 operator*(double scale, const Vec2& vec)
{
	return operator*(vec, scale);
}
