#pragma once

#include "vec.h"
#include "Ray.h"
#include "Materials/Material.h"

#include <memory>

class hit_record {
public:
	vec3 p;
	vec3 normal;
	std::shared_ptr<Material> material;
	double t;
	bool front_face;

	// void set(hit_record other) {
	// 	this.p = other.p.copy();
	// 	this.normal = other.normal.copy();
	// 	this.material = other.material;
	// 	this.t = other.t;
	// 	this.front_face = other.front_face;
	// }

	void set_face_normal(Ray r, vec3 outward_normal) {
		front_face = dot(r.dir, outward_normal) < 0;
		normal = front_face ? outward_normal : (outward_normal * -1);
	}
};