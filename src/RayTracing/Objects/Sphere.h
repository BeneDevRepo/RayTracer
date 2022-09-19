#pragma once

#include <memory>

#include "RayTracing/vec.h"
#include "RayTracing/Ray.h"
#include "RayTracing/hit_record.h"

#include "RayTracing/Objects/hittable.h"

#include "RayTracing/Materials/Material.h"

class Sphere : public hittable {
	const vec3 center;
	const double radius;
	std::shared_ptr<Material> material;

public:
	// Sphere() {}
	Sphere(const point3& center, const double r, std::shared_ptr<Material>& m)
		: center(center), radius(r), material(m) {};

	virtual bool hit(const Ray& r, const double t_min, const double t_max, hit_record& rec) const override {
		const vec3 oc = r.orig - center; // from sphere-center to ray Origin
		const double a = length_squared(r.dir);
		const double half_b = dot(oc, r.dir);
		const double c = length_squared(oc) - radius*radius;

		const double discriminant = half_b*half_b - a*c;
		if (discriminant < 0)
			return false;

		const double sqrtd = sqrt(discriminant);

		// Find the nearest root that lies in the acceptable range.
		double root = (-half_b - sqrtd) / a;
		if (root < t_min ||  root > t_max) {
			root = (-half_b + sqrtd) / a;
			if (root < t_min || t_max < root)
				return false;
		}

		rec.t = root;
		rec.p = r.at(rec.t);
		const vec3 outward_normal = (rec.p - center) / radius;
		rec.set_face_normal(r, outward_normal);
		rec.material = material;

		return true;
	}
};
