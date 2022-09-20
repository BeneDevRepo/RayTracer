#include "fTexture.h"

void swap(auto& a, auto& b) {
	const auto tmp = a;
	a = b;
	b = tmp;
}

fTexture::fTexture():
		pixels(nullptr),
		width(0),
		height(0) {
}

fTexture::fTexture(const uint32_t width, const uint32_t height):
		pixels(new uint32_t[width * height]),
		width(width),
		height(height) {
}

fTexture::fTexture(fTexture&& other): fTexture() {
	swap(pixels, other.pixels);
	swap(width, other.width);
	swap(height, other.height);
}

fTexture& fTexture::operator=(fTexture&& other) {
	if(this == &other) return *this;
	swap(pixels, other.pixels);
	swap(width, other.width);
	swap(height, other.height);
	return *this;
}

fTexture::~fTexture() {
	delete[] pixels;
}