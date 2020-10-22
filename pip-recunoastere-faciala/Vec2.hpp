#pragma once


class Vec2
{
public:
	int x, y;

	Vec2(int x = 0, int y = 0);
	Vec2(const Vec2&);
	Vec2& operator=(const Vec2&);

	Vec2 operator+(const Vec2&);
	Vec2 operator-(const Vec2&);
};