#pragma once

unsigned char* negateImage(unsigned char* img, int w, int h)
{
	unsigned char* result = new unsigned char[w*h];
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			result[y*w + x] = 255 - img[y*w + x];
	return result;
}