#pragma once

#include <memory>
#include <vector>

#include "RayTracing/vec.h"
#include "RayTracing/Ray.h"
#include "RayTracing/hit_record.h"

#include "RayTracing/Objects/hittable.h"
#include "RayTracing/Objects/Triangle.h"

#include "RayTracing/Materials/Material.h"


class Mesh : public hittable {
	const std::vector<Triangle> mesh;
	// std::shared_ptr<Material> material;
	// const vec3 center;
	// const double radius;

public:
	Mesh(const std::vector<Triangle>& mesh/*, std::shared_ptr<Material>& material*/)
		: mesh(mesh)/*, material(material)*/ {};

	virtual bool hit(const Ray& r, const double t_min, const double t_max, hit_record& rec) const override {
		hit_record temp_rec;
		bool hit_anything = false;
		auto closest_so_far = t_max;

		vec3 center(0.f, 0.f, 0.f);
		for(const Triangle& tri : mesh)
			center += (tri.p0 + tri.p1 + tri.p2) / 3.f;
		center /= mesh.size();

		float maxDistSQ = 0.f;
		for(const Triangle& tri : mesh) {
			vec3 verts[3] { tri.p0, tri.p1, tri.p2 };
			for(int i = 0; i < 3; i++) {
				const vec3 diff = verts[i] - center;
				const float distSQ = dot(diff, diff);
				if(distSQ > maxDistSQ)
					maxDistSQ = distSQ;
			}
		}

		{
			const vec3 L = center - r.orig; // rayOrig to sphere
			float tca = dot(L, normalize(r.dir));
			// if (tca < 0) return false;
			float dSquared = dot(L, L) - tca * tca;

			if(dSquared > maxDistSQ)
				return false;
		}



		for(const Triangle& tri : mesh) {
			if(tri.hit(r, t_min, closest_so_far, temp_rec)) {
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}

		// for (const auto& object : objects) {
		// 	if (object->hit(r, t_min, closest_so_far, temp_rec)) {
		// 		hit_anything = true;
		// 		closest_so_far = temp_rec.t;
		// 		rec = temp_rec;
		// 	}
		// }

		return hit_anything;
	}
};
