#include <iostream>
#include <fstream>
#include "ray.h"

float hit_sphere(const vec3& center, float radius, const ray& r)
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a*c;
	
	if (discriminant < 0)
	{
		return -1.0;
	}
	else
	{
		return (b - sqrt(discriminant)) / (2.0*a);
	}
}

vec3 color(const ray& r)
{
	float t = hit_sphere(vec3(0, 0, -1), 0.5, r);
	if (t > 0.0)
	{
		vec3 N = unit_vector(r.point_at_parameter(t) - vec3(0, 0, -1));
		return 0.5 * vec3(N.x() + 1, N.y() + 1, N.z() + 1);
	}
	vec3 unit_direction = unit_vector(r.direction());
	t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(.5, .7, 1.0);
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
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			float u = float(i) / float(nx);
			float v = float(j) / float(ny);

			ray r(origin, lower_left_corner + u * horz + v * vert);
			vec3 col = color(r);

			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);

			file << ir << " " << ig << " " << ib << "\n";
		}
	}
	file.close();


	return 0;
}