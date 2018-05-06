#include <iostream>
#include <fstream>
#include "ray.h"
#include "material.h"
#include "hitableList.h"
#include "sphere.h"
#include <float.h>
#include "Camera.h"
#include <cstdlib>
#include <time.h>
#include <ppl.h>
#include <vector>
vec3 color(const ray& r, hitable* world, int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec))
	{
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}
	}
	else
	{
		vec3 unit_direction = r.direction().unit_vector();
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(.5, .7, 1.0);
	}
}

int main()
{
	struct RGB {
		int ir, ig, ib;
		RGB() :ir(0), ig(0), ib(0) {};
		RGB(int r, int g, int b) :ir(r), ig(g), ib(b) {};
	};
	using namespace std;
	using namespace concurrency;
	fstream file;
	file.open("result.ppm", fstream::out);

	int nx = 800;
	int ny = 400;
	int ns = 100;
	file << "P3\n" << nx << " " << ny << "\n255\n";
	
	vec3 lower_left_corner(-2, -1, -1);
	vec3 horz(4, 0, 0);
	vec3 vert(0, 2, 0);
	vec3 origin(0, 0, 0);
	std::vector<hitable*> list;

	list.emplace_back(new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5))));
	list.emplace_back(new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0))));
	list.emplace_back(new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0)));
	list.emplace_back(new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5)));
	list.emplace_back(new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5)));
	

	hitable* world = new hitable_list(list.data(), list.size());

	vec3 lookfrom(3, 3, 2);
	vec3 lookat(0, 0, -1);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 2.0;


	Camera cam(lookfrom,lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

	RGB** pixels = new RGB*[ny];
	parallel_for(int(0), ny, [&](int i) {
		pixels[i] = new RGB[nx];
	});
	

	parallel_for(int(0), ny, [&](int j) {

		for (int i = 0; i < nx; i++)
		{
			vec3 col(0, 0, 0);
			srand(time(NULL));
			for (int s = 0; s < ns; s++)
			{
			
				float u = float(i + static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) / float(nx);
				float v = float(j + static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) / float(ny);

				ray r = cam.get_ray(u, v);

				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
			}

			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));


			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);

			pixels[j][i] = RGB(ir, ig, ib);
		}
	
	});
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{

		
			int ir = pixels[j][i].ir;
			int ig = pixels[j][i].ig;
			int ib = pixels[j][i].ib;

			file << ir << " " << ig << " " << ib << "\n";
		}
	}
	file.close();
	for (auto& it : list) {
		delete it;
	}
	parallel_for(int(0), ny, [&](int i) {
		delete pixels[i];
	});
	delete[] pixels;
	return 0;
}