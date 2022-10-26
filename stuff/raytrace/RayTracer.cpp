// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "ObjMaterial.h"
#include "fresnel.h"
#include "RayTracer.h"

using namespace optix;

bool RayTracer::trace_reflected(const Ray& in, const HitInfo& in_hit, Ray& out, HitInfo& out_hit) const {
	// Initialize the reflected ray and trace it.
	//
	// Input:  in         (the ray to be reflected)
	//         in_hit     (info about the ray-surface intersection)
	//
	// Output: out        (the reflected ray)
	//         out_hit    (info about the reflected ray)
	//
	// Return: true if the reflected ray hit anything
	//
	// Hints: (a) There is a reflect function available in the OptiX math library.
	//        (b) Set out_hit.ray_ior and out_hit.trace_depth.


	/*
	* Here I have just done the maths exactly like it was presented on Reflected Rays (week 3, slide 6).
	*/
	if (in_hit.has_hit) {
		/*
		* We have to check whether we actually hit anything, we can use the in_hit "data" for that purpose.
		* 
		* Additionally we should probably assign all the member variables of the "out" (which the function returns).
		* 
		* We should find the negative direction vector, because we need it to be directed towards the light.
		* (But when we use the reflect function, then it isn't needed).
		* 
		* Additionally we should make sure to assign tmin and tmax of our "out" vector.
		*/
		float constexpr epsilon = 0.0001;
		out.origin = in_hit.position;
		out.tmin = epsilon;
		out.tmax = RT_DEFAULT_MAX;
		// out.direction = 2 * dot(-in.direction, in_hit.geometric_normal) * in_hit.geometric_normal + in.direction;
		out.direction = reflect(in.direction, in_hit.geometric_normal);
		

		/*
		* Then we use the closest hit function, to find wether our outgoing Ray "out" actually hit anything.
		*/
		if (scene->closest_hit(out, out_hit)) {
			/*
			* Jakob said that this trace_depth should just be incremented.
			*/
			out_hit.trace_depth = in_hit.trace_depth + 1;

			/*
			* ior = Index of Refraction (see Geometrical Optics - week 3, slide 3).
			* 
			* Now we try using the in_hit.ray_ior, for updating the out_hit.ray_ior.
			*/
			out_hit.ray_ior = in_hit.ray_ior;
			return true;

		}

	}
	

	return false;
}

bool RayTracer::trace_refracted(const Ray& in, const HitInfo& in_hit, Ray& out, HitInfo& out_hit) const {
	// Initialize the refracted ray and trace it.
	//
	// Input:  in         (the ray to be refracted)
	//         in_hit     (info about the ray-surface intersection)
	//
	// Output: out        (the refracted ray)
	//         out_hit    (info about the refracted ray)
	//
	// Return: true if the refracted ray hit anything
	//
	// Hints: (a) There is a refract function available in the OptiX math library.
	//        (b) Set out_hit.ray_ior and out_hit.trace_depth.
	//        (c) Remember that the function must handle total internal reflection.


	if (in_hit.has_hit) {
		/*
		* Start by setting our tmin and tmax.
		*/
		float constexpr epsilon = 0.0001;
		out.origin = in_hit.position;
		out.tmin = epsilon;
		out.tmax = RT_DEFAULT_MAX;

		/*
		* We now go to Refracted Rays (week 3, slide 7).
		* It might actually be Refraction and Total Internal Reflection (week 3, slide 8).
		*
		* n_i = refractive index in
		* n_t = refractive index out
		*/
		float3 some_normal{};

		out_hit.ray_ior = get_ior_out(in, in_hit, some_normal);

		/*
		* (Week 3, slide 8) states that if cos^2 (theta_t) < = 0, then all is reflected and nothing
		* is refracted.
		* 
		* omega_i is the negative of in.direction.
		*/
		float const in_over_out = in_hit.ray_ior / out_hit.ray_ior;
		float const in_over_out2 = in_over_out * in_over_out;
		float const cos_in = dot(-in.direction, in_hit.geometric_normal);
		float const cos_in2 = cos_in * cos_in;
		float const cos_2_out_angle = 1 - in_over_out2 * (1 - cos_in2);

		
		if (0 <= cos_2_out_angle) {
			/*
			* Out direction is here calculated like the very long expression at the botton of Week 3, slide 8.
			*/
			refract(out.direction, in.direction, some_normal, out_hit.ray_ior / in_hit.ray_ior);
			/*
			* out.direction = in_over_out *
				(dot(-in.direction, in_hit.geometric_normal) * in_hit.geometric_normal + in.direction) -
				in_hit.geometric_normal * sqrt(1 - in_over_out2 * (1 - cos_in2));
			*/
			

			/*
			* Then we use the closest hit function, to find wether our outgoing Ray "out" actually hit anything.
			*/
			if (scene->closest_hit(out, out_hit)) {
				/*
				* Jakob said that this trace_depth should just be incremented.
				*/
				out_hit.trace_depth = in_hit.trace_depth + 1;
				return true;

			}

		}

		

	}

	return false;
	

}

bool RayTracer::trace_refracted(const Ray& in, const HitInfo& in_hit, Ray& out, HitInfo& out_hit, float& R) const
{
  // Initialize the refracted ray and trace it.
  // Compute the Fresnel reflectance (see fresnel.h) and return it in R.
  //
  // Input:  in         (the ray to be refracted)
  //         in_hit     (info about the ray-surface intersection)
  //
  // Output: out        (the refracted ray)
  //         out_hit    (info about the refracted ray)
  //
  // Return: true if the refracted ray hit anything
  //
  // Hints: (a) There is a refract function available in the OptiX math library.
  //        (b) Set out_hit.ray_ior and out_hit.trace_depth.
  //        (c) Remember that the function must handle total internal reflection.
  R = 0.1;
  return trace_refracted(in, in_hit, out, out_hit);
}

float RayTracer::get_ior_out(const Ray& in, const HitInfo& in_hit, float3& normal) const
{
  normal = in_hit.shading_normal;
	if(dot(normal, in.direction) > 0.0)
	{
    normal = -normal;
    return 1.0f;
  }
  const ObjMaterial* m = in_hit.material;
  return m ? m->ior : 1.0f;
}
