#pragma once
#include "Vec2.hpp"
#include <cstdint>
#include <memory>
#include <functional>
#include <vector>
#include <QImage>

using uchar = std::uint8_t;

class Image
{
private:
	static QVector<QRgb> grayLUT;
	std::vector<uchar> img;
	Vec2 dim;
public:
	Image(const char* fileName);
	Image(Vec2 dim);

	Image(const Image&);
	Image& operator=(const Image&);

	uchar& operator[](int);
	const uchar& operator[](int) const;

	Vec2 dims() const;
	const uchar* get() const;
	uchar* get();
	int width() const;
	int height() const;

	std::shared_ptr<QImage> get_qimage() const;
};