#pragma once

#include "RayTracing/vec.h"
#include "RayTracing/Ray.h"
#include "RayTracing/hit_record.h"
#include "RayTracing/color.h"

#include "RayTracing/Materials/Material.h"

class Lambertian : public Material {
	const color albedo;

public:
	Lambertian(color albedo): albedo(albedo) { }

	virtual bool scatter(const Ray& r_in, const hit_record& rec, color& attenuation, Ray& scattered) const override {
		vec3 scatter_direction = rec.normal + random_unit_vector(); // TODO random scattering

		// Catch degenerate scatter direction
		if (near_zero(scatter_direction))
			scatter_direction = rec.normal;

		scattered.orig = rec.p;
		scattered.dir = scatter_direction;
		attenuation = albedo;
		return true;
	}
};