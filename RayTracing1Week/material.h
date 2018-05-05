#pragma once
#include "hitable.h"
class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered)const = 0;
};

class lambertian : public material {
public:
	lambertian(const vec3& a) :albedo(a) {}
	vec3 albedo;
	// material을(를) 통해 상속됨
	virtual bool scatter(const ray & r_in, const hit_record & rec, vec3 & attenuation, ray & scattered) const override;
};
class metal : public material {
public:
	metal(const vec3& a, float f) :albedo(a) { f < 1 ? fuzz = f : fuzz = 1; }
	vec3 albedo;
	float fuzz;

	// material을(를) 통해 상속됨
	virtual bool scatter(const ray & r_in, const hit_record & rec, vec3 & attenuation, ray & scattered) const override;
};
