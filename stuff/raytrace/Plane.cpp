// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "Plane.h"

using namespace optix;

bool Plane::intersect(const Ray& r, HitInfo& hit, unsigned int prim_idx) const {
	// Implement ray-plane intersection here.
	// It is fine to intersect with the front-facing side of the plane only.
	//
	// Input:  r                    (the ray to be checked for intersection)
	//         prim_idx             (index of the primitive element in a collection, not used here)
	//
	// Output: hit.has_hit          (set true if the ray intersects the plane)
	//         hit.dist             (distance from the ray origin to the intersection point)
	//         hit.position         (coordinates of the intersection point)
	//         hit.geometric_normal (the normalized normal of the plane)
	//         hit.shading_normal   (the normalized normal of the plane)
	//         hit.material         (pointer to the material of the plane)
	//        (hit.texcoord)        (texture coordinates of intersection point, not needed for Week 1)
	//
	// Return: True if the ray intersects the plane, false otherwise 
	//         (do not return hit.has_hit as it is shared between all primitives and remains true once it is set true)
	//
	// Relevant data fields that are available (see Plane.h and OptiX math library reference)
	// r.origin                     (ray origin)
	// r.direction                  (ray direction)
	// r.tmin                       (minimum intersection distance allowed)
	// r.tmax                       (maximum intersection distance allowed)
	// position                     (origin of the plane)
	// onb                          (orthonormal basis of the plane: normal [n], tangent [b1], binormal [b2])
	// d                            (displacement of the plane, d in the equation of the plane)
	// material                     (material of the plane)
	//
	// Hint: The OptiX math library has a function dot(v, w) which returns
	//       the dot product of the vectors v and w.

	/*
	* Here we use the equation as defined in Ray-plane intersection (can be found in the slides from week 2).
	* I am quite certain that "r.direction" is equal to the normalized w-vector as described in slides.
	*/
	float const t_prime = -(dot(r.origin, onb.m_normal) + d) / dot(r.direction, onb.m_normal);

	/*
	* We define the result that we need to return.
	*/
	bool did_it_hit = false;

	/*
	* Then we have the following conditional that we want to check.
	* So in order:
	* - If we have hit we set has_hit to true.
	* - The distance to the intersection is t_prime, that is what we isolated.
	* - The position of intersection must be how far we traverse along the direction from the origin.
	* - I assume that the geometric_normal and the shading_normal are both normal to the plane.
	* - Finally we need to dereference the material for some reason..!?
	*/
	if (r.tmin <= t_prime && t_prime <= r.tmax) {
		did_it_hit = true;

		hit.has_hit = did_it_hit;
		hit.dist = t_prime;
		hit.position = r.origin + r.direction * t_prime;

		/*
		* Probably 
		*/
		// float3 const normalized_normal = onb.m_normal / sqrt(dot(onb.m_normal, onb.m_normal));
		float3 const normalized_normal = normalize(onb.m_normal);
		hit.geometric_normal = normalized_normal;
		hit.shading_normal = normalized_normal;
		hit.material = &material;

		/*
		* We need to check here the material actually has a texture, if that is the case
		* then hit has some coordinates that should be assigned:
		*/
		if (material.has_texture) {
			get_uv(hit.position, hit.texcoord.x, hit.texcoord.y);
		}


	}


	return did_it_hit;
}

void Plane::transform(const Matrix4x4& m)
{
  onb = Onb(normalize(make_float3(m*make_float4(onb.m_normal, 0.0f))));
  position = make_float3(m*make_float4(position, 1.0f));
  d = -dot(position, onb.m_normal);
}

Aabb Plane::compute_bbox() const
{
  return Aabb(make_float3(-1e37f), make_float3(1e37f));
}

void Plane::get_uv(const float3& hit_pos, float& u, float& v) const { 
	// Do an inverse mapping from hit position to texture coordinates.
	//
	// Input:  hit_pos    (position where a ray intersected the plane)
	//
	// Output: u          (texture coordinate in the tangent direction of the plane)
	//         v          (texture coordinate in the binormal direction of the plane)
	//
	// Relevant data fields that are available (see Plane.h and OptiX math library reference)
	// position           (origin of the plane)
	// onb                (orthonormal basis of the plane: normal [n], tangent [b1], binormal [b2])
	// tex_scale          (constant for scaling the texture coordinates)

	/*
	* hit_pos := must be equal to the vector which is called x in the slides
	* (See Texture coordinates - slides 4, week 4).
	* 
	* position := suspects that this is the original x, so x_0.
	* (See Texture coordinates - slides 4, week 4).
	*/

	float3 const x_x0 = hit_pos - position;
	

	/*
	* The assignment (and this function) states that we should multiply tex_scale onto
	* our texture coordinates (...for scaling!?)
	*/
	u = tex_scale * dot(onb.m_tangent, x_x0);
	v = tex_scale * dot(onb.m_binormal, x_x0);
}