#pragma once

namespace Tmpl8 {
	class Camera
	{
	public:
		Camera();
		vec3 position = vec3(0, 0, 0);
		vec3 direction = vec3(0, 0, -1);
		float rotation = 0;
		float screenDistance = 1;
	};
};