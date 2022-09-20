#pragma once

#include <cstdint>

class fTexture {
public:
	uint32_t *pixels;
	int width, height;

	fTexture();
	fTexture(const uint32_t width, const uint32_t height);
	fTexture(fTexture&& other);
	fTexture& operator=(fTexture&& other);
	~fTexture();
};