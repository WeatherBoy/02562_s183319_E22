// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "Phong.h"

using namespace optix;

#ifndef M_1_PIf
#define M_1_PIf 0.31830988618379067154
#endif

float3 Phong::shade(const Ray& r, HitInfo& hit, bool emit) const {
	float3 rho_d = get_diffuse(hit);
	float3 rho_s = get_specular(hit);
	float s = get_shininess(hit);

	// Implement Phong reflection here.
	//
	// Input:  r          (the ray that hit the material)
	//         hit        (info about the ray-surface intersection)
	//         emit       (passed on to Emission::shade)
	//
	// Return: radiance reflected to where the ray was coming from
	//
	// Relevant data fields that are available (see Lambertian.h, Ray.h, and above):
	// lights             (vector of pointers to the lights in the scene)
	// hit.position       (position where the ray hit the material)
	// hit.shading_normal (surface normal where the ray hit the material)
	// rho_d              (difuse reflectance of the material)
	// rho_s              (specular reflectance of the material)
	// s                  (shininess or Phong exponent of the material)
	//
	// Hint: Call the sample function associated with each light in the scene.

	/*
	* These two can be initialized to nothing as they will be overwritten within the
	* sample-function.
	*/
	float3 dir{};
	float3 light_incident{};
	float3 result = make_float3(0.0f);
	float3 w_r{};


	for (unsigned int i = 0; i < lights.size(); ++i) {
		/*
		* We go through every single light source and see if they hit (which
		* they do as our sample function only returns true for now).
		*/
		if (lights[i]->sample(hit.position, dir, light_incident)) {
			/*
			* dir := is the direction towards the lights
			* (meaning that -dir is the direction from the light towards the point)
			* 
			* Then we add those light sources to the result as a cumulative sum.
			* 
			* w_i from slides is the in_direction (this I would assume be the same as the direction
			* of the ray that hit the material).
			* I would assume w_o to be negative w_i in the way that it is phrased.
			* 
			* We use fmax to clamp the dot product (because we do not want the result to become negative):
			*		dot(-r.direction, w_r)
			* 
			* By dividing
			*		rho_s * (s + 2)
			* with two (2), we can then add divide by Pi when returning the result.
			*/
			w_r = reflect(-dir, hit.shading_normal);
			result += (rho_d + rho_s * (s + 2) * 0.5 * pow( fmax(dot(-r.direction, w_r), 0), s ) ) * light_incident * dot(dir, hit.shading_normal);

		}

	}

	return M_1_PIf * result + Emission::shade(r, hit, emit);
}
