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
#define DRAND48 static_cast <float> (rand()) / static_cast <float> (RAND_MAX)
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
std::vector<hitable*> random_scene()
{
	int n = 500;
	std::vector<hitable*> list;
	list.reserve(n + 1);

	list.emplace_back(new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5))));
	int i = 1;
	for(int a = -11; a < 11; a++)
	{
		for(int b = -11; b < 11; b++)
		{
			float chose_mat = DRAND48;
			vec3 center(a + 0.9 * DRAND48, 0.2, b + 0.9 * DRAND48);
			if((center - vec3(4, 0.2, 0)).length() > 0.9)
			{
				if(chose_mat < 0.8)
				{
					list.emplace_back(new sphere(center, 0.2, new lambertian(vec3(DRAND48 *DRAND48, DRAND48*DRAND48, DRAND48*DRAND48))));
				}
				else if(chose_mat < 0.95)
				{
					list.emplace_back(new sphere(center, 0.2, new metal(vec3(0.5 * (1 + DRAND48), 0.5 * (1 + DRAND48), 0.5*(1 + DRAND48)), 0.5*DRAND48)));
				}
				else
				{
					list.emplace_back(new sphere(center, 0.2, new dielectric(1.5)));
				}
			}
		}

		
	}

	list.emplace_back(new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5)));
	list.emplace_back(new sphere(vec3(-4, 1, 0), 1, new lambertian(vec3(0.4, 0.2, 0.1))));
	list.emplace_back(new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0)));
	return list;
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

	int nx = 3200;
	int ny = 1600;
	int ns = 100;
	file << "P3\n" << nx << " " << ny << "\n255\n";
	
	vec3 lower_left_corner(-2, -1, -1);
	vec3 horz(4, 0, 0);
	vec3 vert(0, 2, 0);
	vec3 origin(0, 0, 0);
	std::vector<hitable*> list = random_scene();

// 	list.emplace_back(new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5))));
// 	list.emplace_back(new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0))));
// 	list.emplace_back(new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0)));
// 	list.emplace_back(new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5)));
// 	list.emplace_back(new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5)));
	

	hitable* world = new hitable_list(list.data(), list.size());

	vec3 lookfrom(10, 1, 7);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 0.0;


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