#pragma once

#include <cstdint>

#include "vec.h"

inline uint32_t intColor(const color& col) {
	return	static_cast<uint32_t>(255.999 * col.x()) << 16 |
			static_cast<uint32_t>(255.999 * col.y()) << 8 |
			static_cast<uint32_t>(255.999 * col.z());
}