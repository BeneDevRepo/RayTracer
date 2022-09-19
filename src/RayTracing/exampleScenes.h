#pragma once

#include "RayTracing/Objects/hittable.h"
#include "RayTracing/Objects/hittable_list.h"

#include "RayTracing/Objects/Sphere.h"
#include "RayTracing/Objects/Triangle.h"

void genScene1(hittable_list& world) {
	std::shared_ptr<Material> material_ground = std::make_shared<Lambertian>(color(0.8, 0.8, 0.0));

	// std::shared_ptr<Material> material_center = std::make_shared<Lambertian>(color(0.1, 0.2, 0.5)); // center
	std::shared_ptr<Material> material_center = std::make_shared<Metal>(color(0.9, 0.9, 0.9), .01);
	// std::shared_ptr<Material> material_center = std::make_shared<Dielectric>(1.5);

	// std::shared_ptr<Material> material_left   = std::make_shared<Metal>(color(0.8, 0.8, 0.8), .3); // left
	std::shared_ptr<Material> material_left   = std::make_shared<Dielectric>(1.5);

	std::shared_ptr<Material> material_right  = std::make_shared<Metal>(color(0.8, 0.6, 0.2), .8); // .8 // right

	world.add(std::make_shared<Sphere>(vec3( 0.0, 100.5, 1.0), 100.0, material_ground));
	world.add(std::make_shared<Sphere>(vec3( 0.0, 0.0, 1.0), 0.5, material_center));

	world.add(std::make_shared<Sphere>(vec3(-1.0, 0.0, 1.0), 0.5, material_left));
	// world.add(std::make_shared<Sphere>(vec3(-1.0, 0.0, 1.0), -0.4, material_left)); // hollow Cavity

	world.add(std::make_shared<Sphere>(vec3( 1.0, 0.0, 1.0), 0.5, material_right));
}

void genScene2(hittable_list& world) {
	// ground sphere:
	// std::shared_ptr<Material> ground_material = std::make_shared<Lambertian>(color(0.5, 0.5, 0.5));
	// world.add(std::make_shared<Sphere>(vec3(0, 1000, 0), 1000, ground_material));

	// ton of spheres:
	for (int a = -4; a < 4; a++) {
		for (int b = -4; b < 4; b++) {
	// for (int a = -11; a < 11; a++) {
	// 	for (int b = -11; b < 11; b++) {
			double choose_mat = random_double(0, 1);
			vec3 center = vec3(a + 0.9*random_double(0, 1.), -0.2, b + 0.9*random_double(0, 1.));

			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				std::shared_ptr<Material> sphere_material;

				if (choose_mat < .8) { // .8
					// diffuse
					color albedo = color(random_double(0, 1.)*random_double(0, 1.), random_double(0, 1.)*random_double(0, 1.), random_double(0, 1.)*random_double(0, 1.));
					sphere_material = std::make_shared<Lambertian>(albedo);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				} else if (choose_mat < .95) { // .95
					// metal
					color albedo = color(random_double(0.5, 1.), random_double(0.5, 1.), random_double(0.5, 1.));
					double fuzz = random_double(0, 0.5);
					sphere_material = std::make_shared<Metal>(albedo, fuzz);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				} else {
					// glass
					sphere_material = std::make_shared<Dielectric>(1.5);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}


	// 3 distinctive spheres:
	// std::shared_ptr<Material> material1 = std::make_shared<Dielectric>(1.5);
	// world.add(std::make_shared<Sphere>(vec3(0, -1, 0), 1.0, material1));

	// std::shared_ptr<Material> material2 = std::make_shared<Lambertian>(color(0.4, 0.2, 0.1));
	// world.add(std::make_shared<Sphere>(vec3(-4, -1, 0), 1.0, material2));

	// std::shared_ptr<Material> material3 = std::make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
	// world.add(std::make_shared<Sphere>(vec3(4, -1, 0), 1.0, material3));
}