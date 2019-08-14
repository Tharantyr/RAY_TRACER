#include "precomp.h"

Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, Material material) : Primitive()
{
	Triangle::v0 = v0;
	Triangle::v1 = v1;
	Triangle::v2 = v2;
	Triangle::material = material;
	Triangle::normal = normalize(cross(v1 - v0, v2 - v0));
};

bool Triangle::Intersect(shared_ptr<Ray> ray)
{
	vec3 v0v1 = v1 - v0;
	vec3 v0v2 = v2 - v0;
	vec3 pvec = cross(ray->direction, v0v2);
	float det = dot(v0v1, pvec);

	if (det < 1e-5 && !ray->shadow)
		return false;

	if (abs(det) < 1e-5 && !ray->shadow)
		return false;

	float invDet = 1 / det;

	vec3 tvec = ray->origin - v0;
	float u = dot(tvec, pvec) * invDet;

	if (u < 0 || u > 1)
		return false;

	vec3 qvec = cross(tvec, v0v1);
	float v = dot(ray->direction, qvec) * invDet;

	if (v < 0 || u + v > 1)
		return false;

	float t = dot(v0v2, qvec) * invDet;

	if (t < ray->t)
	{
		ray->t = t;
		return true;
	}

	return false;
}

vec3 Triangle::GetNormal(shared_ptr<Ray> ray)
{
	return normal;
}

Sphere::Sphere(vec3 position, float radius, Material material) : Primitive()
{
	Sphere::position = position;
	Sphere::radius = radius;
	Sphere::material = material;
}

bool Sphere::Intersect(shared_ptr<Ray> ray)
{
	vec3 L = ray->origin - position;
	float a = dot(ray->direction, ray->direction);
	float b = 2 * dot(ray->direction, L);
	float c = dot(L, L) - radius * radius;
	
	float t0, t1;
	if (!SolveQuadratic(a, b, c, t0, t1))
		return false;

	if (t0 < 0)
		t0 = t1;

	if (t0 < 0)
		return false;

	if (t0 < ray->t)
	{
		ray->t = t0;
		return true;
	}

	return false;
}

bool Sphere::SolveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = -0.5f * b / a;
	else {
		float q = (b > 0) ?
			-0.5f * (b + sqrt(discr)) :
			-0.5f * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1) std::swap(x0, x1);
	return true;
}

vec3 Sphere::GetNormal(shared_ptr<Ray> ray)
{
	return normalize((ray->origin + ray->direction * ray->t) - position);
};

Plane::Plane(vec3 position, vec3 normal, Material material) : Primitive()
{
	Plane::position = position;
	Plane::normal = normal;
	Plane::material = material;
}

bool Plane::Intersect(shared_ptr<Ray> ray)
{
	float denom = dot(normal, ray->direction);

	if (abs(denom) > 1e-6)
	{
		vec3 p0l0 = position - ray->origin;
		float t = dot(p0l0, normal) / denom;

		if (t >= 0 && t < ray->t)
		{
			ray->t = t;
			return true;
		}
	}

	return false;
}