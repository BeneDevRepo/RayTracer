#pragma once

#include "RayTracing/vec.h"
#include "RayTracing/Ray.h"
#include "RayTracing/hit_record.h"
#include "RayTracing/color.h"

#include "RayTracing/Materials/Material.h"

class Metal : public Material {
	color albedo;
	double fuzz; // reflection randomization

public:
	Metal(color albedo, double fuzz): albedo(albedo), fuzz(fuzz) { }

	virtual bool scatter(const Ray& r_in, const hit_record& rec, color& attenuation, Ray& scattered) const override {
		vec3 reflected = reflect(unit_vector(r_in.dir), rec.normal);
		scattered.orig = rec.p;
		scattered.dir = reflected + (random_in_unit_sphere() * fuzz); // TODO random scattering
		attenuation = albedo;
		return true;
	}
};