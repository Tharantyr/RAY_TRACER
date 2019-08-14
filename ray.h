#pragma once

namespace Tmpl8 {
	class Ray {
	public:
		Ray(vec3 origin, vec3 direction);
		Ray(vec3 origin, vec3 direction, bool shadow);
		vec3 origin, direction;
		float t = 9999;
		vec3 i = vec3(0); // For Beer's Law; set to 0 initially, set to 1 upon entering material
		bool shadow = false;
	};
};