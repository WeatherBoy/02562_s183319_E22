// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "IndexedFaceSet.h"
#include "ObjMaterial.h"
#include "mt_random.h"
#include "cdf_bsearch.h"
#include "HitInfo.h"
#include "AreaLight.h"

using namespace optix;

bool AreaLight::sample(const float3& pos, float3& dir, float3& L) const {
	const IndexedFaceSet& normals = mesh->normals;
	L = make_float3(0.0f);

	// Compute output and return value given the following information.
	//
	// Input:  pos  (the position of the geometry in the scene)
	//
	// Output: dir  (the direction toward the light)
	//         L    (the radiance received from the direction dir)
	//
	// Return: true if not in shadow
	//
	// Relevant data fields that are available (see Light.h and above):
	// shadows             (on/off flag for shadows)
	// tracer              (pointer to ray tracer)
	// normals             (indexed face set of vertex normals)
	// mesh->face_areas    (array of face areas in the light source)
	//
	// Hints: (a) Use mesh->compute_bbox().center() to get the center of 
	//        the light source bounding box.
	//        (b) Use the function get_emission(...) to get the radiance
	//        emitted by a triangle in the mesh.

	/*
	* Using mt_random_half_open(), because technically we do not want 1 included, because
	* if we do zero-indexing, then we don't want to reach the total amount of entries
	* in "geometry.face"s cause this would create an out of index error.
	*/
	int const rand_indx = static_cast<int>(mt_random_half_open() * mesh->geometry.no_faces());
	// std::cout << rand_indx << std::endl;
	uint3 const rand_vert_indx = mesh->geometry.face(rand_indx);

	double const xi_1 = mt_random_half_open();
	double const xi_2 = mt_random_half_open();

	float const sqrt_xi_1 = sqrt(xi_1);
	float const u = 1 - sqrt_xi_1;
	float const v = (1 - xi_2) * sqrt_xi_1;
	float const w = xi_2 * sqrt_xi_1;



	/*
	* Finding the light position as described in Sampling a Triangle Mesh
	* (Week 8, slide 21).
	* 
	* I know it doesn't make sense to define qi (for i = 0..2), but it looks nicer.
	*/
	float3 const q0 = mesh->geometry.vertex(rand_vert_indx.x);
	float3 const q1 = mesh->geometry.vertex(rand_vert_indx.y);
	float3 const q2 = mesh->geometry.vertex(rand_vert_indx.z);
	float3 const light_pos = q0 * u + q1 * v + q2 * w;

	/*
	* Defining a lot of variables that are used for calculations at a later point.
	*/
	float3 const temp_dir = light_pos - pos;
	float const dir_dot = dot(temp_dir, temp_dir);
	float const dir_length = sqrt(dir_dot);
	dir = temp_dir / dir_length;
	/*
	* REMEMBER! When normalizing you should divdide by the length of the vector,
	* i.e. you should find the square-root of the dot-product of the vector..
	* or just use normalize.
	*/


	/*
	* We make a shadow ray, using the information provided by Shadow Rays
	* (Week 3, slide 5).
	* 
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

	if (tracer->trace_to_any(shadow_ray, hit)) {
		return false;

	}

	/*
	* We calculate the light radiance as described in Approximation for a Distant Area Light
	* (week 5, slide 10).
	*/
	for (unsigned int i = 0; i < mesh->geometry.no_faces(); i++) {
		L += dot(-dir, normalize(mesh->normals.vertex(i))) * get_emission(i) * mesh->face_areas[i];

	}
	L /= (dir_length * dir_length);

	return true;
}

bool AreaLight::emit(Ray& r, HitInfo& hit, float3& Phi) const
{
  // Generate and trace a ray carrying radiance emitted from this area light.
  //
  // Output: r    (the new ray)
  //         hit  (info about the ray-surface intersection)
  //         Phi  (the flux carried by the emitted ray)
  //
  // Return: true if the ray hits anything when traced
  //
  // Relevant data fields that are available (see Light.h and Ray.h):
  // tracer              (pointer to ray tracer)
  // geometry            (indexed face set of triangle vertices)
  // normals             (indexed face set of vertex normals)
  // no_of_faces         (number of faces in triangle mesh for light source)
  // mesh->surface_area  (total surface area of the light source)
  // r.origin            (starting position of ray)
  // r.direction         (direction of ray)

  // Get geometry info
  const IndexedFaceSet& geometry = mesh->geometry;
	const IndexedFaceSet& normals = mesh->normals;
	const float no_of_faces = static_cast<float>(geometry.no_faces());

  // Sample ray origin and direction
 
  // Trace ray
  
  // If a surface was hit, compute Phi and return true

  return false;
}

float3 AreaLight::get_emission(unsigned int triangle_id) const
{
  const ObjMaterial& mat = mesh->materials[mesh->mat_idx[triangle_id]];
  return make_float3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
}
