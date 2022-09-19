#pragma once

#include "RayTracing/vec.h"
#include "RayTracing/Ray.h"
#include "RayTracing/hit_record.h"
#include "RayTracing/color.h"

#include "RayTracing/Materials/Material.h"

class Dielectric : public Material {
	double ir; // Index of Refraction

public:
	Dielectric(double ir): ir(ir) { }

	virtual bool scatter(const Ray& r_in, const hit_record& rec, color& attenuation, Ray& scattered) const override {
		attenuation = color(1., 1., 1.);
		double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

		vec3 unit_direction = unit_vector(r_in.dir);
		double cos_theta = std::min(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

		bool cannot_refract = refraction_ratio * sin_theta > 1.0;

		constexpr auto urand = []()->uint32_t{ return rand(); };

		vec3 direction;
		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > urand()%10000/10000.)
			direction = reflect(unit_direction, rec.normal);
		else
			direction = refract(unit_direction, rec.normal, refraction_ratio);

		scattered.orig = rec.p;
		scattered.dir = direction;

		return true;
	}

	static vec3 refract(const vec3& uv, const vec3& n, const double etai_over_etat) {
		double cos_theta = std::min(dot(-uv, n), 1.0);
		vec3 r_out_perp =  ((n * cos_theta) + uv) * etai_over_etat;
		vec3 r_out_parallel = n * (-std::sqrt(std::abs(1.0 - length_squared(r_out_perp))));
		return r_out_perp + r_out_parallel;
	}

	static double reflectance(double cosine, double ref_idx) {
		// Use Schlick's approximation for reflectance.
		double r0 = (1-ref_idx) / (1+ref_idx);
		r0 = r0*r0;
		return r0 + (1-r0)*pow((1 - cosine), 5.);
	}
};