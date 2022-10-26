// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <sstream>
#include <string>
#include <optix_world.h>
#include "HitInfo.h"
#include "Directional.h"

using namespace std;
using namespace optix;

bool Directional::sample(const float3& pos, float3& dir, float3& L) const {
	// Compute output and return value given the following information.
	//
	// Input:  pos (the position of the geometry in the scene)
	//
	// Output: dir (the direction toward the light)
	//         L   (the radiance received from the direction dir)
	//
	// Return: true if not in shadow
	//
	// Relevant data fields that are available (see Directional.h and Light.h):
	// shadows    (on/off flag for shadows)
	// tracer     (pointer to ray tracer)
	// light_dir  (direction of the emitted light)
	// emission   (radiance of the emitted light)


	/*
	* Jakob said this was exactly like PointLight's sample, so I will blindly obey.
	* 
	* We make a shadow ray, using the information provided by Shadow Rays
	* (Week 3, slide 5).
	* But there are some slight modifications here from PointLight. This directional 
	* light source is something like the sun, where the light is like a plane,
	* all of it comes at the same angle. Which means that our t_max is just the
	* default t_max (because it's from a far away source).
	* 
	* This constexpr, aparently states that it can be calculated during
	* compile-time, this should primarily aid in making the code more easily
	* readable.
	*/
	float constexpr epsilon = 0.0001;
	float constexpr t_max = RT_DEFAULT_MAX;
	dir = -light_dir;
	Ray shadow_ray = make_Ray(pos, dir, 0, epsilon, t_max);

	/*
	* We initialize an empty hit, which our tracer can use. It should be noted,
	* that the "trace_to_any()" function just return the result of the "closest_hit()" function...
	*/
	HitInfo hit{};

	if (tracer->trace_to_any(shadow_ray, hit)) {
		return false;

	}

	/*
	* Apparently the radiance can just be calculated as the emission.
	*/
	L = emission;


	return true;
}

string Directional::describe() const
{
  ostringstream ostr;
  ostr << "Directional light (emitted radiance " << emission << ", direction " << light_dir << ").";
  return ostr.str();
}
