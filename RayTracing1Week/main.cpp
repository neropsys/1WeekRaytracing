#include <iostream>
#include <fstream>
#include "ray.h"
#include "hitableList.h"
#include "sphere.h"
#include <float.h>
vec3 color(const ray& r, hitable* world)
{
	hit_record rec;
	if (world->hit(r, 0, FLT_MAX, rec))
	{
		return 0.5 * vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
	}
	else
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(.5, .7, 1.0);
	}
}

int main()
{
	using namespace std;
	
	fstream file;
	file.open("result.ppm", fstream::out);

	int nx = 800;
	int ny = 400;
	file << "P3\n" << nx << " " << ny << "\n255\n";
	
	vec3 lower_left_corner(-2, -1, -1);
	vec3 horz(4, 0, 0);
	vec3 vert(0, 2, 0);
	vec3 origin(0, 0, 0);
	hitable* list[2];

	list[0] = new sphere(vec3(0, 0, -1), 0.5);
	list[1] = new sphere(vec3(0, -100.5, -1), 100);
	hitable* world = new hitable_list(list, 2);
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			float u = float(i) / float(nx);
			float v = float(j) / float(ny);

			ray r(origin, lower_left_corner + u * horz + v * vert);
		
			vec3 p = r.point_at_parameter(2.0);
			vec3 col = color(r, world);

			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);

			file << ir << " " << ig << " " << ib << "\n";
		}
	}
	file.close();


	return 0;
}