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
float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);

}
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
	vec3 uv = v.unit_vector();

	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);

	if (discriminant > 0) {
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	return false;
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

bool dielectric::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
{
	vec3 outward_normal;
	vec3 reflected = reflect(r_in.direction(), rec.normal);
	float ni_over_nt;

	attenuation = vec3(1.0, 1.0, 1.0);
	vec3 refracted;
	float reflect_prob;
	float cosine;
	if (dot(r_in.direction(), rec.normal) > 0) {
		outward_normal = -rec.normal;
		ni_over_nt = ref_idx;
		cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();

	}
	else {
		outward_normal = rec.normal;
		ni_over_nt = 1.0 / ref_idx;
		cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();

	}
	if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
		reflect_prob = schlick(cosine, ref_idx);
	}
	else {
		scattered = ray(rec.p, reflected);
		reflect_prob = 1.0;
	}
	if (DRAND48 < reflect_prob) {
		scattered = ray(rec.p, reflected);
	}
	else {
		scattered = ray(rec.p, refracted);
	}
	return true;
}
