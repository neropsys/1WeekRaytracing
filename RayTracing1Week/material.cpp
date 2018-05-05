#include "material.h"
#define DRAND48 static_cast <float> (rand()) / static_cast <float> (RAND_MAX)
vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0 * vec3(DRAND48, DRAND48, DRAND48) - vec3(1, 1, 1);

	} while (p.length_sqrt() >= 1.0);
	return p;
}
vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n)*n;
}

bool lambertian::scatter(const ray & r_in, const hit_record & rec, vec3 & attenuation, ray & scattered) const
{
	vec3 target = rec.p + rec.normal + random_in_unit_sphere();
	scattered = ray(rec.p, target - rec.p);
	attenuation = albedo;
	return true;
}

bool metal::scatter(const ray & r_in, const hit_record & rec, vec3 & attenuation, ray & scattered) const
{
	vec3 reflected = reflect(r_in.direction().unit_vector(), rec.normal);
	scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());

	attenuation = albedo;
	return (dot(scattered.direction(), rec.normal) > 0);
}
