#pragma once

namespace Tmpl8 {
	class Primitive
	{
	public:
		virtual bool Intersect(shared_ptr<Ray> ray) = 0;
		virtual Material GetMaterial() = 0;
		virtual vec3 GetNormal(shared_ptr<Ray> ray) = 0;
	};

	class Triangle : public Primitive
	{
	public:
		Triangle(vec3 v0, vec3 v1, vec3 v2, Material material);
		virtual bool Intersect(shared_ptr<Ray> ray) override;
		virtual Material GetMaterial() override { 
			return material; 
		};
		virtual vec3 GetNormal(shared_ptr<Ray> ray) override;
		vec3 v0, v1, v2, normal;
		Material material;
	};

	class Sphere : public Primitive
	{
	public:
		Sphere(vec3 position, float radius, Material material);
		virtual bool Intersect(shared_ptr<Ray> ray) override;
		virtual Material GetMaterial() override { return material; };
		virtual vec3 GetNormal(shared_ptr<Ray> ray) override;
		bool SolveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);
		vec3 position;
		float radius;
		Material material;
	};

	class Plane : public Primitive
	{
	public:
		Plane(vec3 position, vec3 normal, Material material);
		virtual bool Intersect(shared_ptr<Ray> ray) override;
		virtual Material GetMaterial() override { return material; };
		virtual vec3 GetNormal(shared_ptr<Ray> ray) override { return normal; };
		vec3 position, normal;
		Material material;
	};

	class Light
	{
	public:
		Light(vec3 p, float i, vec3 c) { position = p;	intensity = i; color = c; };
		vec3 position;
		float intensity;
		vec3 color;
	};
};