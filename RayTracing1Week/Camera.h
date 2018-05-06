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
	Camera(vec3 lookFrom, vec3 lookAt, vec3 vup, float vFov, float aspect)
	{
		vec3 u, v, w;
		float theta = vFov * M_PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		origin = lookFrom;

		w = (lookFrom - lookAt).unit_vector();
		u = cross(vup, w).unit_vector();
		v = cross(w, u);
		lower_left_corner = vec3(-half_width, -half_height, -1);
		lower_left_corner = origin - half_width * u - half_height * v - w;
		horizontal = 2 * half_width * u;
		vertical = 2 * half_height * v;
	}
	ray get_ray(float u, float v)
	{
		return ray(origin, lower_left_corner +u * horizontal + v * vertical - origin);
	}


};