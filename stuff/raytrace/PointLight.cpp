// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "mt_random.h"
#include "PointLight.h"

using namespace optix;

bool PointLight::sample(const float3& pos, float3& dir, float3& L) const {
	// Compute output and return value given the following information.
	//
	// Input:  pos (the position of the geometry in the scene)
	//
	// Output: dir (the direction toward the light)
	//         L   (the radiance received from the direction dir)
	//
	// Return: true if not in shadow
	//
	// Relevant data fields that are available (see PointLight.h and Light.h):
	// shadows    (on/off flag for shadows)
	// tracer     (pointer to ray tracer)
	// light_pos  (position of the point light)
	// intensity  (intensity of the emitted light)
	//
	// Hint: Construct a shadow ray using the Ray datatype. Trace it using the
	//       pointer to the ray tracer.

	
	

	/*
	* Defining a lot of variables that are used for calculations at a later point.
	*/
	float3 const temp_dir = light_pos - pos;
	float const dir_dot = dot(temp_dir, temp_dir);
	float const dir_length = sqrt(dir_dot);
	dir = (light_pos - pos) / dir_length;
	/*
	* REMEMBER! When normalizing you should divdide by the length of the vector,
	* i.e. you should find the square-root of the dot-product of the vector..
	* or just use normalize.
	*/
	

	/*
	* We make a shadow ray, using the information provided by Shadow Rays
	* (Week 3, slide 5).
	* This constexpr, aparently states that it can be calculated during
	* compile-time, this should primarily aid in making the code more easily
	* readable.
	*/
	float constexpr epsilon = 0.0001;
	Ray shadow_ray = make_Ray(pos, dir, 0, epsilon, dir_length - epsilon);

	/*
	* We initialize an empty hit, which our tracer can use. It should be noted,
	* that the "trace_to_any()" function just return the result of the "closest_hit()" function...
	*/
	HitInfo hit{};
	
	if (shadows && tracer->trace_to_any(shadow_ray, hit)) {
		return false;

	}

	/*
	* Using Kepler's inverse square law to calculate the "light incident"..
	*/
	L = intensity / dir_dot;
	

	return true;
}

bool PointLight::emit(Ray& r, HitInfo& hit, float3& Phi) const {
	// Emit a photon by creating a ray, tracing it, and computing its flux.
	//
	// Output: r    (the photon ray)
	//         hit  (the photon ray hit info)
	//         Phi  (the photon flux)
	//
	// Return: true if the photon hits a surface
	//
	// Relevant data fields that are available (see PointLight.h and Light.h):
	// tracer     (pointer to ray tracer)
	// light_pos  (position of the point light)
	// intensity  (intensity of the emitted light)
	//
	// Hint: When sampling the ray direction, use the function
	//       mt_random() to get a random number in [0,1].

	// Sample ray direction and create ray

	// Trace ray
  
	// If a surface was hit, compute Phi and return true

	/*
	* Defining epsilon. It needs be a new value everytime:
	*/
	float constexpr epsilon = 0.0000001;

	/*
	* As Jakob described, we sample a random vector within a cube,
	* but what we want to do is sampling a random vector within a sphere
	* since a sphere makes up quite a significant part of a cube,
	* we can just sample from the cube untill we are within the sphere.
	* 
	* See week 8, slide 12.
	*/
	do {
		r.direction.x = 2.0f * mt_random() - 1.0f;
		r.direction.y = 2.0f * mt_random() - 1.0f;
		r.direction.z = 2.0f * mt_random() - 1.0f;

	} while (dot(r.direction, r.direction) > 1.0f);

	/*
	* Fills in the ray with information.
	*/
	r.direction = normalize(r.direction);
	r.tmin = epsilon;
	r.tmax = RT_DEFAULT_MAX;
	r.ray_type = 0;
	r.origin = light_pos;
	

	if (tracer->trace_to_closest(r, hit)) {
		/*
		* Finally we find phi as described on:
		* Photon emission from an isotropic point light
		* (Week 8, slide 11).
		*/
		Phi = 4 * M_PIf * intensity;

		return true;

	}

	return false;
}

