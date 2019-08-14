#include "precomp.h"

Ray::Ray(vec3 origin, vec3 direction)
{
	Ray::origin = origin;
	Ray::direction = direction;
};

Ray::Ray(vec3 origin, vec3 direction, bool shadow)
{
	Ray::origin = origin;
	Ray::direction = direction;
	Ray::shadow = shadow;
};