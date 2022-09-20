#include <iostream>
#include <cstdint>
#include <cmath>

#include <string>
#include <ostream>

// #include "FileUtils/FileUtils.h"

// #include "Threading/mingw.thread.h"
// #include "Threading/mingw.mutex.h"
#include <thread>
#include <mutex>

#include "BWindow/GDIWindow.h"
// #include "Window/GDIWindowCustom.h"

#include "RayTracing/general.h"

#include "RayTracing/vec.h"
#include "RayTracing/color.h"
#include "RayTracing/Ray.h"
#include "RayTracing/Camera.h"

#include "RayTracing/Objects/hittable.h"
#include "RayTracing/Objects/hittable_list.h"

#include "RayTracing/Objects/Sphere.h"
// #include "RayTracing/Objects/Triangle.h"
#include "RayTracing/Objects/Mesh.h"

#include "RayTracing/Materials/Lambertian.h"
#include "RayTracing/Materials/Metal.h"
#include "RayTracing/Materials/Dielectric.h"

#include "RayTracing/Texture/fTexture.h"

#include "RayTracing/exampleScenes.h"

#include "stb/stb_image.h"





inline uint32_t pixelColor(const vec3 uv, const vec3 pixelSize, const hittable_list& world, const fTexture& skybox, const Camera& cam, const uint32_t SAMPLES_PER_PIXEL, const uint32_t MAX_NUM_BOUNCES, const uint32_t x, const uint32_t y) {
	color pixel_color{};

	for (uint32_t s = 0; s < SAMPLES_PER_PIXEL; s++) {
		const vec3 screenPos = uv + vec3(random_double(0, 1), random_double(0, 1), 0) * pixelSize;

		const Ray r = cam.getRay(screenPos.x(), screenPos.y());

		pixel_color += ray_color(r, world, skybox, MAX_NUM_BOUNCES);
	}

	const double scale = 1. / SAMPLES_PER_PIXEL;
	pixel_color[0] = sqrt(scale * pixel_color.x());
	pixel_color[1] = sqrt(scale * pixel_color.y());
	pixel_color[2] = sqrt(scale * pixel_color.z());

	return intColor(pixel_color);
	// return skybox.pixels[
	// 	((size_t)((uv.y() * .5 + .5) * skybox.height))
	// 	* skybox.width
	// 	+ ((size_t)((uv.x() * .5 + .5) * skybox.width))
	// ];
}

std::atomic_uint32_t SAMPLES_PER_PIXEL = 1;
std::atomic_uint32_t MAX_NUM_BOUNCES = 8;

fTexture tex(400, 400);

std::atomic_uint32_t lastLine = 0;
std::atomic_uint32_t threadsWorking = 0;
void renderThread(volatile bool* stopThread, volatile Camera *camRef, const hittable_list& world, const fTexture& skybox, volatile bool *idle) {
	// static std::atomic_uint32_t lastLine = 0;

	for(;;) {
		// LARGE_INTEGER before, after;
		// QueryPerformanceCounter(&before);
		// for(; !running; );

		uint32_t targetLine = lastLine.fetch_add(1);// % tex.height;

		// if(targetLine == tex.height - 1)
		// 	lastLine.fetch_sub(tex.height);

		++threadsWorking;
		if(targetLine > tex.height - 1) {
			--threadsWorking;
			// *idle = true;
			continue;
		}

		// *idle = false;

		for (uint32_t x = 0; x < tex.width; x++) {
			if(*stopThread)
				return;

			Camera cam = *(Camera*)camRef;

			tex.pixels[targetLine * tex.width + x] = pixelColor(
					vec3(x*1./tex.width - .5, targetLine*1./tex.width - .5, 0),
					vec3(1. / tex.width, 1. / tex.height, 0),
					world,
					skybox,
					cam,
					SAMPLES_PER_PIXEL.load(),
					MAX_NUM_BOUNCES.load(),
					x,
					targetLine);
			// tex.pixels[targetLine * tex.width + x] = skybox.pixels[
			// 	(targetLine * skybox.height / tex.height)
			// 	* skybox.width
			// 	+ (x * skybox.width / tex.width)
			// ];
		}

		// QueryPerformanceCounter(&after);
		// double elapsed = (after.QuadPart - before.QuadPart) / 10.; // micros
		// std::cout << "Line took: " << (elapsed / 1000.) << "ms\n";
	}
}

int main2(int argc, char** argv);

int main(int argc, char** argv) {
	system("pwd");
	try {
		return main2(argc, argv);
	} catch(const std::exception& ex) {
		std::cout << "Unhandled Exception: " << ex.what() << "\n";
	}
}

int main2(int argc, char** argv) {
	std::cout << "Program started\n";
	fTexture skybox;

	{
		int nChannelsSkybox;
		int width, height;
		stbi_set_flip_vertically_on_load(false); // dont flip loaded textures on the y-axis.

		uint8_t *const data = stbi_load("res/Desert_Highway/Road_to_MonumentValley_8k.jpg", &width, &height, &nChannelsSkybox, 0);
		// uint8_t *const data = stbi_load("../res/Desert_Highway/Road_to_MonumentValley_Ref.hdr", &width, &height, &nChannelsSkybox, 0);
		// uint8_t *const data = stbi_load("../res/OutdoorHDRI024_4K-HDR.exr", &width, &height, &nChannelsSkybox, 0);
		// uint8_t *const data = stbi_load("../res/full-seamless-spherical-hdri-panorama-degrees-angle-view-wooden-pier-near-lake-evening-equirectangular-projection-159712935.jpg", &width, &height, &nChannelsSkybox, 0);

		if (!data)
			throw std::runtime_error("Failed to load skybox image file");

		// std::cout << "channels: " << nChannelsSkybox << "\n";

		std::cout << "Loading Skybox\n";
		skybox = fTexture(width, height);
		std::cout << "Loaded Skybox\n";

		for(size_t y = 0; y < height; y++)
			for(size_t x = 0; x < width; x++)
				skybox.pixels[y * width + x] = intColor( // todo: blur
						vec3(
							data[(y * width + x) * 3 + 0],
							data[(y * width + x) * 3 + 1],
							data[(y * width + x) * 3 + 2]
						) / 255.
					);

		stbi_image_free(data);
	}

	volatile bool stopThreads = false;

	// -- Simple Scene
	// vec3 camPos(0, 0, -2);
	// vec3 camDir(0, 0, 1);
	// float camFOV = 40;
	// float focusDist = 1.;
	// float aperture = 0;
	// hittable_list world;
	// genScene1(world);

	// -- Complex Scene
	vec3 camPos = vec3(13, -2, 3);
	vec3 camDir = unit_vector(-camPos);
	float camFOV = 20; // 100
	float focusDist = 10.;
	float aperture = 0;//0.1;
	hittable_list world;

	genScene2(world);

	// Flat mirror:
	// std::shared_ptr<Material> material4 = std::make_shared<Metal>(color(1., .75, .75), .2);
	// // std::shared_ptr<Material> material4 = std::make_shared<Metal>(color(1., .75, .75), .0);
	// world.add(std::make_shared<Triangle>(vec3(4, 0, 2), vec3(4, -2, 2), vec3(6, 0, 2), material4));
	// world.add(std::make_shared<Triangle>(vec3(6, -2, 2), vec3(4, -2, 2), vec3(6, 0, 2), material4));

	std::shared_ptr<Material> material5 = std::make_shared<Metal>(color(1., .75, .75), .03);
	// std::shared_ptr<Material> material5 = std::make_shared<Metal>(color(1., .75, .75), .0);
	// std::shared_ptr<Dielectric> material5 = std::make_shared<Dielectric>(1.5);
	{
		FILE *const fp = fopen("res/Bunny.stl", "rb");

		if(fp == nullptr)
			throw std::runtime_error("Error reading STL File.");

		uint8_t header[80];
		fread(header, 1, 80, fp);

		uint32_t numTris;
		fread(&numTris, 4, 1, fp);
		printf("Number of Triangles: %d\n", numTris);

		struct StlTriangle {
			vec3 normal;
			vec3 vertices[3];

			uint16_t attrCount;
		};

		StlTriangle *triangles = new StlTriangle[numTris];

		// read triangles:
		for(size_t i = 0; i < numTris; i++) {
			fread(&triangles[i].normal.x(), sizeof(float), 1, fp);
			fread(&triangles[i].normal.y(), sizeof(float), 1, fp);
			fread(&triangles[i].normal.z(), sizeof(float), 1, fp);

			for(size_t vert = 0; vert < 3; vert++) {
				fread(&triangles[i].vertices[vert].x(), sizeof(float), 1, fp);
				fread(&triangles[i].vertices[vert].y(), sizeof(float), 1, fp);
				fread(&triangles[i].vertices[vert].z(), sizeof(float), 1, fp);
			}

			fread(&triangles[i].attrCount, sizeof(uint16_t), 1, fp);
		}

		fclose(fp);

		// correct coordinates:
		for(int i = 0; i < numTris; i++) {
			for(int j = 0; j < 3; j++) {
				std::swap(triangles[i].vertices[j].y(), triangles[i].vertices[j].z());
				triangles[i].vertices[j].y() *= -1;
			}
		}

		std::vector<Triangle> mesh;

		for(int i = 0; i < numTris; i++) {
			vec3 v0{ triangles[i].vertices[0].x(), triangles[i].vertices[0].y(), triangles[i].vertices[0].z() };
			vec3 v1{ triangles[i].vertices[1].x(), triangles[i].vertices[1].y(), triangles[i].vertices[1].z() };
			vec3 v2{ triangles[i].vertices[2].x(), triangles[i].vertices[2].y(), triangles[i].vertices[2].z() };
			// world.add(std::make_shared<Triangle>(v0 * .01, v1 * .01, v2 * .01, material4));
			mesh.push_back(Triangle(v0 * .01, v1 * .01, v2 * .01, material5));
		}

		world.add(std::make_shared<Mesh>(std::move(mesh)));

		delete[] triangles;
	}

	// fTexture tex(800, 800);
	// tex = fTexture(800, 800);
	// renderTarget = &tex;
	Camera cam(camPos, camDir, vec3(0, 1, 0), camFOV, 1, aperture, focusDist);

	static constexpr uint16_t NUM_THREADS = 6;
	std::thread *renderThreads[NUM_THREADS];
	volatile bool idleThreads[NUM_THREADS];
	for(uint16_t t = 0; t < NUM_THREADS; t++)
		renderThreads[t] = new std::thread(renderThread, &stopThreads, &cam, std::cref(world), std::cref(skybox), idleThreads + t);

	GDIWindow win(800, 800);
	// GDIWindowCustom win(800, 800);
	int32_t pmouseX, pmouseY;

	for(;;) {
		win.pollMsg();

		if(win.shouldClose())
			break;

		// if(tex.width != win.width/2 || tex.height != win.height/2) {
		// 	while(lastLine < tex.height);
		// 	for(;;) {
		// 		bool idle = true;
		// 		for(int t = 0; t < NUM_THREADS; t++)
		// 			if(idleThreads[t] == false)
		// 				idle = false;
		// 		if(idle)
		// 			break;
		// 	}
		// 	delete[] tex.pixels;
		// 	tex.width = win.width / 2;
		// 	tex.height = win.height / 2;
		// 	tex.pixels = new uint32_t[tex.width * tex.height];
		// 	lastLine = 0;
		// }

		if(lastLine > tex.height)
			lastLine = 0;

		int32_t mouseX = win.win.mouseX;
		int32_t mouseY = win.win.mouseY;

		constexpr float moveSpeed = .02;

		if (GetAsyncKeyState('W') & 0x8000)
    		camPos += (unit_vector(camDir) * (moveSpeed));
		if (GetAsyncKeyState('S') & 0x8000)
			camPos += (unit_vector(camDir) * (-moveSpeed));
		
		if (GetAsyncKeyState('D') & 0x8000)
			camPos += (unit_vector(cam.horizontal) * (moveSpeed));
		if (GetAsyncKeyState('A') & 0x8000)
			camPos += (unit_vector(cam.horizontal) * (-moveSpeed));

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			camPos += (unit_vector(cam.vertical) * (moveSpeed));
		if (GetAsyncKeyState(' ') & 0x8000)
			camPos += (unit_vector(cam.vertical) * (-moveSpeed));

		if (GetAsyncKeyState('R') & 0x0001)
			camFOV += 5;
		if (GetAsyncKeyState('F') & 0x0001)
			camFOV -= 5;

		if (GetAsyncKeyState('E') & 0x0001)
			SAMPLES_PER_PIXEL = SAMPLES_PER_PIXEL.load() * 2;
		if (GetAsyncKeyState('Q') & 0x0001)
			if (SAMPLES_PER_PIXEL > 1)
				SAMPLES_PER_PIXEL = SAMPLES_PER_PIXEL.load() / 2;

		if (GetAsyncKeyState('T') & 0x8000)
			focusDist += .1;
		if (GetAsyncKeyState('G') & 0x8000)
			focusDist -= focusDist>3 ? .1 : 0;

		if (GetAsyncKeyState('Z') & 0x8000)
			aperture += .003;
		if (GetAsyncKeyState('H') & 0x8000)
			aperture -= aperture>=.01 ? .003 : 0;

		// if(GetAsyncKeyState(VK_MBUTTON) & 0x8000) {
		if(GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
			vec3 up(0, 1, 0);

			float dX = mouseX - pmouseX;
			float dY = mouseY - pmouseY;

			float aX = atan2(camDir.z(), camDir.x()); // left / right tilt
			float aY = acos(dot(up, camDir)); // up * camDir = cos(aY) <=> acos(up * camDir) = aY
			aY += radians(dY) / 10.;
			camDir[1] = cos(aY);
			camDir[2] = sin(aY);
			{
				float mX = vec3(0, camDir.x(), camDir.z()).length();
				aX -= radians(dX) / 10.;
				camDir[2] = sin(aX) * mX;
				camDir[0] = cos(aX) * mX;
			}
		}

		cam = Camera(camPos, camDir, vec3(0, 1, 0), camFOV, 1, aperture, focusDist);

		// stretch rendertexture
		// for (int y = 0; y < win.height; y++) {
		// 	for (int x = 0; x < win.width; x++) {
		// 		const uint32_t texIndex = (y*tex.height/win.height) * tex.width + (x*tex.width/win.width);
		// 		win.graphics.buffer[(y) * win.width + x] = tex.pixels[texIndex];
		// 	}
		// }

		// fill overhang with bars
		const bool fillHorizontal = (win.width* 1. / win.height) > (tex.width * 1. / tex.height);
		for (int y = 0; y < win.height; y++) {
			for (int x = 0; x < win.width; x++) {
				if(fillHorizontal) {
					const float texScale = win.height * 1. / tex.height;

					const int wLeft = (win.width - tex.width*texScale) / 2;
					const int xTex = x - wLeft;

					if(xTex < 0 || xTex >= tex.width * texScale) {
						win.graphics.buffer[y * win.width + x] = 0x00;
						continue;
					}

					const uint32_t texIndex = (y * tex.height/win.height) * tex.width + (xTex * tex.height/win.height);

					win.graphics.buffer[(y) * win.width + x] = tex.pixels[texIndex];
				} else {
					const float texScale = win.width * 1. / tex.width;

					const int hTop = (win.height - tex.height*texScale) / 2;
					const int yTex = y - hTop;

					if(yTex < 0 || yTex >= tex.height * texScale) {
						win.graphics.buffer[y * win.width + x] = 0x00;
						continue;
					}

					const uint32_t texIndex = (yTex * tex.width/win.width) * tex.width + (x * tex.width/win.width);

					win.graphics.buffer[y * win.width + x] = tex.pixels[texIndex];
				}
			}
		}

		// win.graphics.clear(0xFF666666); // A R G B
		// win.graphics.fillCircle(mouseX, mouseY, 5, 0xFF00FF00);

		win.updateScreen();

		pmouseX = mouseX;
		pmouseY = mouseY;
	}

	stopThreads = true;

	for(size_t i = 0; i < NUM_THREADS; i++) {
		renderThreads[i]->join();
		delete renderThreads[i];
	}

	std::cout << "Reached end without crashing!\n";
	return 0;
}