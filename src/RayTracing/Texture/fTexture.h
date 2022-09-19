#pragma once

#include <cstdint>

class fTexture {
public:
	uint32_t *pixels;
	int width, height;

	fTexture(const uint32_t width=0, const uint32_t height=0);
	fTexture(fTexture&& other);
	fTexture& operator=(fTexture&& other);
	~fTexture();
};