#pragma once

namespace Tmpl8 {
	class Renderer
	{
	public:
		Renderer();
		void Render();
		vec3 Trace(shared_ptr<Ray> ray, int depth);
		void Init(Surface* screen);
		void HandleInput(int key);
		float CreateRGB(int r, int b, int g);
		vec3 Reflect(vec3 vector, vec3 normal);
		vec3 Refract(vec3 vector, vec3 normal, float index);
		float Fresnel(vec3 vector, vec3 normal, float index);
		vec3 Diffuse(shared_ptr<Ray> ray, vec3 hit, vec3 normal, vec3 color, shared_ptr<Primitive> prim, vec3 spec, vector<shared_ptr<Primitive>>::iterator i);
		void RenderThread(int i, int blockX, int blockY);
		Surface* screen;
		shared_ptr<Camera> camera;
		vector<shared_ptr<Primitive>> primList;
		vector<shared_ptr<Light>> lights;
		float scrWidth;
		float scrHeight;
		float iAR;
		int blockWidth, blockHeight, columns, rows;
	};
}