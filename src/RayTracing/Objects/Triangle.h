#pragma once

#include <memory>

#include "RayTracing/vec.h"
#include "RayTracing/Ray.h"
#include "RayTracing/hit_record.h"

#include "RayTracing/Objects/hittable.h"

#include "RayTracing/Materials/Material.h"

class Triangle : public hittable {
	std::shared_ptr<Material> material;

public:
	const vec3 p0, p1, p2;

public:
	// Triangle() {}
	Triangle(const point3& p0, const point3& p1, const point3& p2, std::shared_ptr<Material> m)
		: p0(p0), p1(p1), p2(p2), material(m) {};

	virtual bool hit(const Ray& r, const double t_min, const double t_max, hit_record& rec) const override {
		const float kEpsilon = .00000001;

		const vec3& v0 = p0;
		const vec3& v1 = p1;
		const vec3& v2 = p2;
		// const vec3& v1 = p2;
		// const vec3& v2 = p1;

		const vec3 v0v1 = v1 - v0;
		const vec3 v0v2 = v2 - v0;

		// no need to normalize
		const vec3 N = cross(v0v1, v0v2); // N
		const float area2 = N.length();

		// Step 1: finding P
 
		// check if ray and plane are parallel ?
		const float NdotRayDirection = dot(N, r.dir); 
		if (fabs(NdotRayDirection) < kEpsilon) // almost 0 
			return false; // they are parallel so they don't intersect ! 
	
		// compute d parameter using equation 2
		const float d = -dot(N, v0);
	
		// compute t (equation 3)
		const float t = -(dot(N, r.orig) + d) / NdotRayDirection;
	
		// check if the triangle is in behind the ray
		if (t < 0.00001) return false; // the triangle is behind

		if(t > t_max) return false; // Already found a closer object
	
		// compute the intersection point using equation 1
		const vec3 P = r.orig + t * r.dir; 
	
		// Step 2: inside-outside test
		vec3 C; // vector perpendicular to triangle's plane 

		// edge 0
		const vec3 edge0 = v1 - v0;
		const vec3 vp0 = P - v0;
		C = cross(edge0, vp0);
		if (dot(N, C) < 0) return false; // P is on the right side 
	
		// edge 1
		const vec3 edge1 = v2 - v1; 
		const vec3 vp1 = P - v1; 
		C = cross(edge1, vp1); 
		if (dot(N, C) < 0) return false; // P is on the right side 
	
		// edge 2
		const vec3 edge2 = v0 - v2;
		const vec3 vp2 = P - v2;
		C = cross(edge2, vp2); 
		if (dot(N, C) < 0) return false; // P is on the right side; 

		rec.set_face_normal(r, N / N.length());
		// rec.set_face_normal(r, N);
		rec.material = material;
		rec.p = P;
		rec.t = t;
	
		return true; // this ray hits the triangle 



		// rec.t = root;
		// rec.p = r.at(rec.t);
		// vec3 outward_normal = (rec.p - p0) / .5;
		// rec.set_face_normal(r, outward_normal);
		// rec.material = material;

		// return true;
	}
};
