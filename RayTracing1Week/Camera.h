#pragma once

#include "ray.h"
#define _USE_MATH_DEFINES
#include <math.h>
class Camera {
public:
	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	float lens_radius;
	vec3 u, v, w;
	Camera(vec3 lookFrom, vec3 lookAt, vec3 vup, float vFov, float aspect, float aperture, float focus_dist)
	{
		lens_radius = aperture / 2;
		float theta = vFov * M_PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		origin = lookFrom;

		w = (lookFrom - lookAt).unit_vector();
		u = cross(vup, w).unit_vector();
		v = cross(w, u);
		lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
		horizontal = 2 * half_width * focus_dist* u;
		vertical = 2 * half_height * focus_dist * v;
	}
	ray get_ray(float s, float t);

};