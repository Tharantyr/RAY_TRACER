#pragma once

namespace Tmpl8 {
	enum class Mat { Diffuse, Glass, Mirror, Partial };

	class Material
	{
	public:
		Material() { material = Mat::Diffuse; color = vec3(255, 0, 0); };
		Material(Mat material, vec3 color);
		Mat material;
		vec3 color;
		float kd = 1;
		float ks = 0;
		float ri = 1; // Refractive index
		vec3 a = vec3(1); // Absorption rate
	};
}