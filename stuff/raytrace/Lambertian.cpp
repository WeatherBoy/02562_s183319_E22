// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "Lambertian.h"

using namespace optix;

// The following macro defines 1/PI
#ifndef M_1_PIf
#define M_1_PIf 0.31830988618379067154
#endif

float3 Lambertian::shade(const Ray& r, HitInfo& hit, bool emit) const {
	float3 const rho_d = get_diffuse(hit);
	float3 result = make_float3(0.0f);
  
	// Implement Lambertian reflection here.
	//
	// Input:  r          (the ray that hit the material)
	//         hit        (info about the ray-surface intersection)
	//         emit       (passed on to Emission::shade)
	//
	// Return: radiance reflected to where the ray was coming from
	//
	// Relevant data fields that are available (see Lambertian.h, HitInfo.h, and above):
	// lights             (vector of pointers to the lights in the scene)
	// hit.position       (position where the ray hit the material)
	// hit.shading_normal (surface normal where the ray hit the material)
	// rho_d              (difuse reflectance of the material)
	//
	// Hint: Call the sample function associated with each light in the scene.

	/*
	* These two can be initialized to nothing as they will be overwritten within the 
	* sample-function.
	*/
	float3 dir{};
	float3 light_incident{};

	for (unsigned int i = 0; i < lights.size(); ++i) {
		/*
		* We go through every single light source and see if they hit (which
		* they do as our sample function only returns true for now).
		*/
		if (lights[i]->sample(hit.position, dir, light_incident)) {
			/*
			* Then we add those light sources to the result as a cumulative sum.
			*/
			result += rho_d * M_1_PIf * light_incident * fmax(0, dot(hit.shading_normal, dir));

		}

	}


	return result + Emission::shade(r, hit, emit);
}
