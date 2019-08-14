#include "precomp.h"
#define MULTI 1 // 1 for multithreading, otherwise 0

Renderer::Renderer()
{
	shared_ptr<Camera> c(new Camera());
	camera = c;
	vector<shared_ptr<Primitive>> p;
	primList = p;
	vector<shared_ptr<Light>> l;
	lights = l;
	scrWidth = SCRWIDTH;
	scrHeight = SCRHEIGHT;

	// Initialize multithreading stuff
	// Columns * rows = total # of threads used
	columns = thread::hardware_concurrency(); // Get optimal number of threads to use
	rows = 1;
	blockWidth = (int)(scrWidth / columns);
	blockHeight = (int)(scrHeight / rows);
}

void Renderer::Init(Surface* s)
{
	screen = s;
	iAR = scrWidth / scrHeight; // Calculate image aspect ratio

	// Test planes (front, left, right, down, up)
	Material material(Mat::Partial, vec3(255, 0, 0));
	shared_ptr<Primitive> plane1(new Plane(vec3(0, 0, -20), vec3(0, 0, 1), material));
	primList.push_back(plane1);
	material.material = Mat::Diffuse;
	material.color = vec3(128, 255, 126);
	shared_ptr<Primitive> plane4(new Plane(vec3(0, -10, 0), vec3(0, 1, 0), material));
	primList.push_back(plane4);

	// Test triangles
	material.color = vec3(250, 0, 250);
	shared_ptr<Primitive> triangle(new Triangle(vec3(-1, 0, -5), vec3(1, 0, -7), vec3(0, 2, -6), material));
	primList.push_back(triangle);

	// Test spheres
	material.color = vec3(50, 50, 200);
	material.material = Mat::Mirror;
	shared_ptr<Primitive> sphere2(new Sphere(vec3(3, -1, -6), 1, material));
	primList.push_back(sphere2);

	material.color = vec3(55, 55, 0);
	material.kd = 0.8;
	material.ks = 0.2;
	material.material = Mat::Glass;
	material.ri = 1.517;
	material.a = 0.4;
	shared_ptr<Primitive> sphere1(new Sphere(vec3(-4, -6, -8), 1.5, material));
	primList.push_back(sphere1);

	material.color = vec3(100, 100, 255);
	material.material = Mat::Diffuse;
	shared_ptr<Primitive> sphere3(new Sphere(vec3(-6, -7, -12), 1, material));
	primList.push_back(sphere3);

	// Test lights
	shared_ptr<Light> light1(new Light(vec3(-2, 8, -1), 0.005f, vec3(255, 255, 255)));
	lights.push_back(light1);
};

float Renderer::CreateRGB(int r, int g, int b)
{
	return (float)((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

void Renderer::RenderThread(int i, int blockX, int blockY)
{
	clock_t start = clock();

	for (int x = blockX * blockWidth; x < (blockX + 1) * blockWidth; x++)
		for (int y = blockY * blockHeight; y < (blockY + 1) * blockHeight; y++)
		{
			float Px = (2 * ((x + 0.5f) / scrWidth) - 1) * iAR; // Calculate world x-coordinate for pixel
			float Py = 1 - 2 * ((y + 0.5f) / scrHeight); // Calculate world y-coordinate for pixel
			vec4 rotation = vec4(Px, Py, -camera->screenDistance, 1) * mat4::rotatey(camera->rotation); // Apply rotation from camera controls
			shared_ptr<Ray> ray(new Ray(camera->position, normalize(vec3(rotation.x, rotation.y, rotation.z)))); // Generate ray in direction of pixel

			int depth = 0;
			vec3 color = Trace(ray, depth); // Shoot ray and get color value based on object(s) hit
			Pixel result = CreateRGB(min(255, color.x), min(255, color.y), min(255, color.z)); // Convert color vector to float value

			screen->Plot(x, y, result); // Plot the actual pixel
		}

	// Print FPS
	double duration = (clock() - start) / CLOCKS_PER_SEC;
	printf("%f", duration);
	printf(" seconds for thread ");
	printf("%d\n", i);
};

void Renderer::Render()
{
	clock_t start = clock();

#if MULTI==1 // Multithreading
	thread t[8];
	for (int y = 0; y < rows; y++)
		for (int x = 0; x < columns; x++)
		{
			t[(columns * y) + x] = thread(&Renderer::RenderThread, this, (columns * y) + x, x, y);
		}

	for (int x = 0; x < columns * rows; x++)
		t[x].join();

#elif MULTI==0 // No multithreading
	for (int x = 0; x < scrWidth; x++)
		for (int y = 0; y < scrHeight; y++)
		{
			float Px = (2 * ((x + 0.5f) / scrWidth) - 1) * iAR; // Calculate world x-coordinate for pixel
			float Py = 1 - 2 * ((y + 0.5f) / scrHeight); // Calculate world y-coordinate for pixel
			vec4 rotation = vec4(Px, Py, -camera->screenDistance, 1) * mat4::rotatey(camera->rotation); // Apply rotation from camera controls
			shared_ptr<Ray> ray(new Ray(camera->position, normalize(vec3(rotation.x, rotation.y, rotation.z)))); // Generate ray in direction of pixel

			int depth = 0;
			vec3 color = Trace(ray, depth); // Shoot ray and get color value based on object(s) hit
			Pixel result = CreateRGB(min(255, color.x), min(255, color.y), min(255, color.z)); // Convert color vector to float value

			screen->Plot(x, y, result); // Plot the actual pixel
		}
#endif

	// Print controls
	screen->Print("Camera controls: ASDWQE   FOV: UP/DOWN   Aspect ratio: brackets", 2, 2, 0xffffff);

	// Print camera position and direction
	string text = "Position: (" + to_string(camera->position.x) + ", " + to_string(camera->position.y) + ", " + to_string(camera->position.z) + ")\n";
	screen->Print(_strdup(text.c_str()), 2, 17, 0xffffff);

	text = "Direction: (" + to_string(camera->direction.x) + ", " + to_string(camera->direction.y) + ", " + to_string(camera->direction.z) + ")\n";
	screen->Print(_strdup(text.c_str()), 2, 32, 0xffffff);

	// Print FPS
	double duration = (clock() - start) / (double)CLOCKS_PER_SEC;
	double fps = 1 / duration;
	text = to_string(fps) + " FPS";
	screen->Print(_strdup(text.c_str()), 2, 47, 0xffffff);
};

vec3 Renderer::Trace(shared_ptr<Ray> ray, int depth)
{
	// If maximum depth reached, break
	if (depth > 2)
		return vec3(50);

	vec3 color = vec3(0);
	vec3 spec = vec3(0);
	shared_ptr<Primitive> prim;
	vector<shared_ptr<Primitive>>::iterator i;

	for (vector<shared_ptr<Primitive>>::iterator it = primList.begin(); it != primList.end(); ++it) // Loop over primitives and test for ray-primitive intersection
	{
		if ((*it)->Intersect(ray))
		{
			prim = *it;
			i = it; // Store primitive index so we can exclude the object when casting shadow rays
		}
	}

	if (prim == NULL)
		return vec3(0);

	vec3 normal = prim->GetNormal(ray);
	vec3 hit = ray->origin + ray->direction * ray->t;

	// If object is reflective and refractive
	if (prim->GetMaterial().material == Mat::Glass)
	{
		vec3 beer = 1;

		// If ray is inside a material (i.e. ray->i != vec3(0)), update light intensity according to Beer's Law
		// Rays' i values are set to vec3(0) by default, we indicate that they went into a material by setting the i value to 1
		if (ray->i.x != 0 && ray->i.y != 0 && ray->i.z != 0)
		{
			beer = ray->i;
			float s = ray->t;
			vec3 a = prim->GetMaterial().a;
			beer.x *= exp(-a.x * s);
			beer.y *= exp(-a.y * s);
			beer.z *= exp(-a.z * s);
		}

		float ri = prim->GetMaterial().ri;
		vec3 refl = normalize(Reflect(ray->direction, normal));
		vec3 refr = normalize(Refract(ray->direction, normal, ri));
		vec3 reflOrigin = (dot(refl, normal) < 0) ? hit - normal * 1e-5 : hit + normal * 1e-5;
		vec3 refrOrigin = (dot(refr, normal) < 0) ? hit - normal * 1e-5 : hit + normal * 1e-5;
		shared_ptr<Ray> reflRay(new Ray(reflOrigin, refl));
		shared_ptr<Ray> refrRay(new Ray(refrOrigin, refr));
		refrRay->i = 1; // For Beer's Law
		vec3 reflColor = Trace(reflRay, depth + 1);
		vec3 refrColor = Trace(refrRay, depth + 1);
		float kr = Fresnel(ray->direction, normal, ri);
		color = prim->GetMaterial().color * 0.2 + (reflColor * kr + refrColor * (1 - kr) * beer) * 0.8;
	}

	// If object is reflective
	else if (prim->GetMaterial().material == Mat::Mirror)
	{
		float kr = Fresnel(ray->direction, normal, prim->GetMaterial().ri);
		vec3 ref = Reflect(ray->direction, normal);
		shared_ptr<Ray> refRay(new Ray(hit + normal * 1e-5, ref));
		color = prim->GetMaterial().color * 0.5 + Trace(refRay, depth + 1) * 0.5;
	}

	// If object is partially reflective
	else if (prim->GetMaterial().material == Mat::Partial)
	{
		color = Diffuse(ray, hit, normal, color, prim, spec, i);
		float kr = Fresnel(ray->direction, normal, prim->GetMaterial().ri);
		vec3 ref = Reflect(ray->direction, normal);
		shared_ptr<Ray> refRay(new Ray(hit + normal * 1e-5, ref));
		vec3 reflColor = Trace(refRay, depth + 1);

		color = color * 0.5 + reflColor * 0.5;
	}

	// If object is diffuse
	else
		color = Diffuse(ray, hit, normal, color, prim, spec, i);

	return color;
};

vec3 Renderer::Diffuse(shared_ptr<Ray> ray, vec3 hit, vec3 normal, vec3 color, shared_ptr<Primitive> prim, vec3 spec, vector<shared_ptr<Primitive>>::iterator i)
{
	for (auto&& l : lights) // Shoot shadow ray at all light sources
	{
		float transmission = 1;

		// Cast shadow ray
		vec3 lightRay = l->position - hit;
		vec3 lightDir = normalize(lightRay);
		vec3 sOrigin = (dot(ray->direction, normal) < 0) ? hit + normal * 1e-5 : hit - normal * 1e-5;
		shared_ptr<Ray> sRay(new Ray(sOrigin, lightDir, true));

		// Loop over primitives and check for intersection with shadow ray
		for (vector<shared_ptr<Primitive>>::iterator it = primList.begin(); it != primList.end(); ++it)
		{
			if (it != i) // Exclude current object to prevent shadow ray from intersecting with the object it originated from (for triangles)
			{
				if ((*it)->Intersect(sRay))
				{
					float d = sqrt(dot(lightRay, lightRay));

					// If distance to intersected object is smaller than distance to light, pixel gets no light, break
					if (sRay->t < d)
					{
						transmission = 0.3;
						break;
					}
				}
			}
		}

		float lightAngle = max(0, dot(normal, lightDir));
		color += prim->GetMaterial().color * transmission * l->color * l->intensity * lightAngle;

		// Specular reflection
		vec3 ref = Reflect(lightDir, normal);
		spec += pow(max(0, dot(ref, ray->direction)), 50) * 1000;
	}
	color = color * prim->GetMaterial().kd + spec * prim->GetMaterial().ks;

	return color;
};

float Renderer::Fresnel(vec3 i, vec3 n, float index)
{
	float cosi = min(1, max(-1, dot(i, n)));
	float etai = 1;
	float etat = index;
	float kr;

	if (cosi > 0)
		swap(etai, etat);

	float sint = etai / etat * sqrt(max(0, 1 - cosi * cosi));

	if (sint >= 1)
		kr = 1;
	else
	{
		float cost = sqrt(max(0, 1 - sint * sint));
		cosi = abs(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
	
	return kr;
};

vec3 Renderer::Reflect(vec3 vector, vec3 normal)
{
	return vector - 2 * dot(vector, normal) * normal;
};

vec3 Renderer::Refract(vec3 i, vec3 n, float ri)
{
	float cosi = min(1, max(-1, dot(i, n)));
	float etai = 1;
	float etat = ri;
	vec3 temp = n;

	if (cosi < 0)
	{
		cosi = -cosi;
	}
	else
	{
		swap(etai, etat);
		temp = -n;
	}

	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? 0 : eta * i + (eta * cosi - sqrt(k)) * temp;
};

void Renderer::HandleInput(int key)
{
	// Angle for rotation
	float rotAngle = PI / 8;

	if (key == SDL_SCANCODE_W) // Move camera forward
		camera->position += camera->direction;

	else if (key == SDL_SCANCODE_S) // Move camera backwards
		camera->position -= camera->direction;

	else if (key == SDL_SCANCODE_D) // Rotate camera to the right
	{
		camera->rotation -= rotAngle; // Keep track of camera's rotation to transform camera rays
		vec4 direction = vec4(camera->direction, 1);
		direction = direction * mat4::rotatey(-rotAngle);
		camera->direction = vec3(direction.x, direction.y, direction.z);
	}

	else if (key == SDL_SCANCODE_A) // Rotate camera to the left
	{
		camera->rotation += rotAngle;
		vec4 direction = vec4(camera->direction, 1);
		direction = direction * mat4::rotatey(rotAngle);
		camera->direction = vec3(direction.x, direction.y, direction.z);
	}

	else if (key == SDL_SCANCODE_E) // Move camera up
		camera->position += vec3(0, 1, 0);

	else if (key == SDL_SCANCODE_Q) // Move camera right
		camera->position += vec3(0, -1, 0);

	else if (key == SDL_SCANCODE_UP) // Decrease FOV
		camera->screenDistance *= 1.1f;

	else if (key == SDL_SCANCODE_DOWN) // Increase FOV
		camera->screenDistance *= 0.91f;

	else if (key == SDL_SCANCODE_LEFTBRACKET) // Decrease aspect ratio
	{
		float newWidth = scrWidth * 0.91f;
		if (newWidth >= scrHeight)
			scrWidth = newWidth;
	}

	else if (key == SDL_SCANCODE_RIGHTBRACKET) // Increase aspect ratio
		scrWidth *= 1.1f;
};