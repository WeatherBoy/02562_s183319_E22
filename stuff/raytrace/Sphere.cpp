// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "Sphere.h"

using namespace optix;

bool Sphere::intersect(const Ray& r, HitInfo& hit, unsigned int prim_idx) const {
	// Implement ray-sphere intersection here.
	//
	// Input:  r                    (the ray to be checked for intersection)
	//         prim_idx             (index of the primitive element in a collection, not used here)
	//
	// Output: hit.has_hit          (set true if the ray intersects the sphere)
	//         hit.dist             (distance from the ray origin to the intersection point)
	//         hit.position         (coordinates of the intersection point)
	//         hit.geometric_normal (the normalized normal of the sphere)
	//         hit.shading_normal   (the normalized normal of the sphere)
	//         hit.material         (pointer to the material of the sphere)
	//        (hit.texcoord)        (texture coordinates of intersection point, not needed for Week 1)
	//
	// Return: True if the ray intersects the sphere, false otherwise
	//
	// Relevant data fields that are available (see Sphere.h and OptiX math library reference)
	// r.origin                     (ray origin)
	// r.direction                  (ray direction)
	// r.tmin                       (minimum intersection distance allowed)
	// r.tmax                       (maximum intersection distance allowed)
	// center                       (sphere center)
	// radius                       (sphere radius)
	// material                     (material of the sphere)
	//
	// Hints: (a) The square root function is called sqrt(x).
	//        (b) There is no need to handle the case where the 
	//            discriminant is zero separately.

	/*
	* As the slides defined (from week 2, slide 6) in Ray-sphere intersection, we need to get
	* both the t_1_prime and t_2_prime. Very interessting...
	* We start by defining some variables, just so we do not have to calculate them multiple
	* times:
	*/
	bool did_it_hit = false;
	float3 const oc = r.origin - center;
	float const b_half = dot(oc, r.direction);
	float const c = dot(oc, oc) - (radius * radius);
	float const b_half2 = sqrt(b_half * b_half - c);

	/*
	* Then we actually calculate the two t_primes.
	*/
	float const t_1_prime = -b_half - b_half2;
	float const t_2_prime = -b_half + b_half2;

	/*
	* Then as we did for the plane we create a conditional, checking whether we
	* are within the desired range.
	*/
	if (r.tmin <= t_1_prime && t_1_prime <= r.tmax) {
		did_it_hit = true;

		hit.has_hit = did_it_hit;
		hit.dist = t_1_prime;
		hit.position = r.origin + r.direction * t_1_prime;

		/*
		* We figured that a normal on the hit position, must be the vector
		* going from the center to the hit_position.
		*/
		float3 const p_minus_center = hit.position - center;
		// float3 const normal_to_sphere = p_minus_center / sqrt(dot(p_minus_center, p_minus_center));
		float3 const normal_to_sphere = normalize(p_minus_center);
		hit.geometric_normal = normal_to_sphere;
		hit.shading_normal = normal_to_sphere;

		hit.material = &material;

	} else if (r.tmin <= t_2_prime && t_2_prime <= r.tmax) {
		did_it_hit = true;

		hit.has_hit = did_it_hit;
		hit.dist = t_2_prime;
		hit.position = r.origin + r.direction * t_2_prime;

		/*
		* We figured that a normal on the hit position, must be the vector
		* going from the center to the hit_position.
		*/
		float3 const p_minus_center = hit.position - center;
		// float3 const normal_to_sphere = p_minus_center / sqrt(dot(p_minus_center, p_minus_center));
		float3 const normal_to_sphere = normalize(p_minus_center);
		hit.geometric_normal = normal_to_sphere;
		hit.shading_normal = normal_to_sphere;

		hit.material = &material;

	}

	return did_it_hit;
}

void Sphere::transform(const Matrix4x4& m)
{
  float3 radius_vec = make_float3(radius, 0.0f, 0.0f) + center;
  radius_vec = make_float3(m*make_float4(radius_vec, 1.0f));
  center = make_float3(m*make_float4(center, 1.0f)); 
  // The radius is scaled by the X scaling factor.
  // Not ideal, but the best we can do without elipsoids
  radius_vec -= center;
  radius = length(radius_vec);  
}

Aabb Sphere::compute_bbox() const
{
  Aabb bbox;
  bbox.include(center - make_float3(radius, 0.0f, 0.0f));
  bbox.include(center + make_float3(radius, 0.0f, 0.0f));
  bbox.include(center - make_float3(0.0f, radius, 0.0f));
  bbox.include(center + make_float3(0.0f, radius, 0.0f));
  bbox.include(center - make_float3(0.0f, 0.0f, radius));
  bbox.include(center + make_float3(0.0f, 0.0f, radius));
  return bbox;
}
