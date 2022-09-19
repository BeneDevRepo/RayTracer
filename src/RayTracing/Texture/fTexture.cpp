#include "fTexture.h"

auto swap(auto& a, auto& b) {
	const auto tmp = a;
	a = b;
	b = tmp;
}

fTexture::fTexture(const uint32_t width, const uint32_t height):
		pixels(
			(width|height) != 0
				? new uint32_t[width * height]
				: nullptr
		),
		width(width),
		height(height) {
}

fTexture::fTexture(fTexture&& other): fTexture() {
	swap(pixels, other.pixels);
	swap(width, other.width);
	swap(height, other.height);
}

fTexture& fTexture::operator=(fTexture&& other) {
	swap(pixels, other.pixels);
	swap(width, other.width);
	swap(height, other.height);
}

fTexture::~fTexture() {
	delete[] pixels;
}