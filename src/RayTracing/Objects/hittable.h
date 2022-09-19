#pragma once

#include "RayTracing/vec.h"
#include "RayTracing/Ray.h"
#include "RayTracing/hit_record.h"

class hittable {
public:
	virtual bool hit(const Ray& r, const double t_min, const double t_max, hit_record& rec) const = 0;
};