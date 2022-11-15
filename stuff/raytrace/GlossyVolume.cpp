// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "int_pow.h"
#include "GlossyVolume.h"

using namespace optix;

#ifndef M_1_PIf
#define M_1_PIf 0.31830988618379067154
#endif

float3 GlossyVolume::shade(const Ray& r, HitInfo& hit, bool emit) const {
	// Compute the specular part of the glossy shader and attenuate it
	// by the transmittance of the material if the ray is inside (as in
	// the volume shader).

	if (hit.trace_depth >= max_depth) {
		return make_float3(0.0f);

	}

	float3 rho_s = get_specular(hit);
	float s = get_shininess(hit);

	float3 dir = make_float3(0.0f);
	float3 light_incident = make_float3(0.0f);
	float3 phong_result = make_float3(0.0f);
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
			phong_result += (pow(fmax(dot(-r.direction, w_r), 0.0), s)) * light_incident * fmax(0.0f, dot(dir, hit.shading_normal));

		}

	}

	phong_result *= M_1_PIf * rho_s * (s + 2) * 0.5;

	/*
	* Finding the transmittance:
	*/
	float3 T_r = make_float3(1.0f);
	if (dot(r.direction, hit.shading_normal) > 0.0f) {
		T_r = Volume::get_transmittance(hit);

	}
	

	float R;
	Ray reflected, refracted;
	HitInfo hit_reflected, hit_refracted;
	tracer->trace_reflected(r, hit, reflected, hit_reflected);
	tracer->trace_refracted(r, hit, refracted, hit_refracted, R);

	float3 const L_0 = R * (phong_result  + shade_new_ray(reflected, hit_reflected))
		+ (1.0f - R) * shade_new_ray(refracted, hit_refracted);

	return T_r * L_0;
}
