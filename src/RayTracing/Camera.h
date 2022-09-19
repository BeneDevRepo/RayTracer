#pragma once

#include "vec.h"
#include "Ray.h"

static inline double radians(const double degrees) noexcept {
	return degrees * 2. * 3.1415926535 / 360.;
}

class Camera {
public:
	vec3 horizontal, vertical, center_of_viewplane;

private:
	vec3 origin;
	double lens_radius, focus_dist;

public:
	Camera(
		vec3 lookfrom,
		vec3 viewDir,
		vec3 vup,
		double vfov, // vertical field-of-view in degrees
		double aspect_ratio = 1,
		double aperture = 0,
		double focus_dist = 1):
			origin(lookfrom),
			lens_radius(aperture/2), focus_dist(focus_dist) {

		double h = tan(radians(vfov) / 2);
		double viewport_height = 2.0 * h;
		double viewport_width = aspect_ratio * viewport_height;

		vec3 w = -unit_vector(viewDir);
		vec3 u = unit_vector(cross(vup, w)); // horizontal
		vec3 v = cross(w, u); // vertical

		horizontal = u * (viewport_width * focus_dist);
		vertical = v * (viewport_height * focus_dist);

		// lower_left_corner = origin*0 - horizontal/2*0 - vertical/2*0 - focus_dist*w;
		center_of_viewplane = -focus_dist * w;
	}

	Camera(const Camera&) = default;
	Camera& operator=(const Camera&) = default;

	Ray getRay(const double s, const double t) const {
		const vec3 rd = lens_radius * random_in_unit_disk();
        const vec3 offset = unit_vector(horizontal) * rd.x() + unit_vector(vertical) * rd.y();

		vec3 pixelPos = center_of_viewplane + horizontal * s + vertical * t - offset;

		return Ray(origin + offset, unit_vector(pixelPos));
	}
};