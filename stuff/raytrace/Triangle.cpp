// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "Triangle.h"

using namespace optix;

bool intersect_triangle(const Ray& ray,
                        const float3& v0,
                        const float3& v1,
                        const float3& v2,
                        float3& n,
                        float& t,
                        float& beta,
                        float& gamma) {
    // Implement ray-triangle intersection here (see Listing 1 in the lecture note).
    // Note that OptiX also has an implementation, so you can get away
    // with not implementing this function. However, I recommend that
    // you implement it for completeness.
    float3 const e0 = v1 - v0;
    float3 const e1 = v2 - v0;

    float const denominator{ dot(ray.direction, n) };

    if (abs(denominator) < 1.0e-8f) {
        return false;
    }

    float q = 1.0f / denominator;
    float3 const o_to_v0 = v0 - ray.origin;

    t = { dot((o_to_v0), n) * q };
    if (!(ray.tmin < t && t < ray.tmax)) {
        return false;
    }

    float3 const n_tmp = cross(o_to_v0, ray.direction);
    beta = dot(n_tmp, e1) * q;

    if (beta < 0.0f) {
        return false;
    }

    gamma = -dot(n_tmp, e0) * q;

    if (gamma < 0.0f || beta + gamma > 1.0f) {
        return false;
    }

    return true;
}


bool Triangle::intersect(const Ray& r, HitInfo& hit, unsigned int prim_idx) const {
    // Implement ray-triangle intersection here.
    //
    // Input:  r                    (the ray to be checked for intersection)
    //         prim_idx             (index of the primitive element in a collection, not used here)
    //
    // Output: hit.has_hit          (set true if the ray intersects the triangle)
    //         hit.dist             (distance from the ray origin to the intersection point)
    //         hit.position         (coordinates of the intersection point)
    //         hit.geometric_normal (the normalized normal of the triangle)
    //         hit.shading_normal   (the normalized normal of the triangle)
    //         hit.material         (pointer to the material of the triangle)
    //        (hit.texcoord)        (texture coordinates of intersection point, not needed for Week 1)
    //
    // Return: True if the ray intersects the triangle, false otherwise
    //
    // Relevant data fields that are available (see Triangle.h)
    // r                            (the ray)
    // v0, v1, v2                   (triangle vertices)
    // (t0, t1, t2)                 (texture coordinates for each vertex, not needed for Week 1)
    // material                     (material of the triangle)
    //
    // Hint: Use the function intersect_triangle(...) to get the hit info.
    //       Note that you need to do scope resolution (optix:: or just :: in front
    //       of the function name) to choose between the OptiX implementation and
    //       the function just above this one.

    /*
    * We calculate the t_prime as it is shown in (week 2, slide 4) Ray-triangle intersection.
    * We start by defining a couple of the vectors as shown on the same slide.
    */
    bool did_it_hit = false;
    float3 const e0 = v1 - v0;
    float3 const e1 = v2 - v0;
    float3 const normal_vector = cross(e0, e1);
    float const t_prime = dot((v0 - r.origin), normal_vector) / dot(r.direction, normal_vector);

    /*
    * As with the previous two intersect functions (sphere and plane),
    * we need to check that we are within the condition.
    */
    if (r.tmin <= t_prime && t_prime <= r.tmax) {
        /*
        * However, unlike those last to intersect functions, we also need to check the
        * Barycentric coordinates.
        * Which we calculate as beta and gamma.
        */
        float3 const v0ow = cross(v0 - r.origin, r.direction);
        float const wn = dot(r.direction, normal_vector);
        float const beta = dot(v0ow, e1) / wn;
        float const gamma = -dot(v0ow, e0) / wn;

        if (0 <= beta && 0 <= gamma && beta + gamma <= 1) {
            did_it_hit = true;
            hit.has_hit = did_it_hit;
            hit.dist = t_prime;
            hit.position = r.origin + r.direction * t_prime;

            /*
            * Maybe the normal vector isn't normalized?
            * Seems weird if that is the case, but now it is at least normalized.
            */
            float3 const normalized_normal = normal_vector / sqrt(dot(normal_vector, normal_vector));
            hit.geometric_normal = normalized_normal;
            hit.shading_normal = normalized_normal;
            hit.material = &material;

        }

    }

    return did_it_hit;
}

void Triangle::transform(const Matrix4x4& m) 
{ 
  v0 = make_float3(m*make_float4(v0, 1.0f)); 
  v1 = make_float3(m*make_float4(v1, 1.0f)); 
  v2 = make_float3(m*make_float4(v2, 1.0f)); 
}

Aabb Triangle::compute_bbox() const
{
  Aabb bbox;
  bbox.include(v0);
  bbox.include(v1);
  bbox.include(v2);
  return bbox;
}
