#include "Camera.h"
#define DRAND48 static_cast <float> (rand()) / static_cast <float> (RAND_MAX)
vec3 random_in_unit_disk() {
	vec3 p;
	do {
		p = 2.0 * vec3(DRAND48, DRAND48, 0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
		return p;

}

ray Camera::get_ray(float s, float t)
{
	vec3 rd = lens_radius * random_in_unit_disk();
	vec3 offset = u * rd.x() + v * rd.y();
	return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
}
