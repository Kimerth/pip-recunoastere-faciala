#pragma once
#include "Image.hpp"
#include <functional>

Image transformImage(const Image& img, std::function<uchar(uchar)> transform)
{
	Image ret(img.dims());

	for (int y = 0; y < ret.height(); y++)
		for (int x = 0; x < ret.width(); x++)
			ret[y * ret.width() + x] = transform(img[y * ret.width() + x]);

	return ret;
}

Image negateImage(const Image& img)
{
	return transformImage(img, [](auto b) {
		return 255 - b;
		});
}