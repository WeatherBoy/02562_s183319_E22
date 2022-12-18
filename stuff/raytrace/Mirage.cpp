// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "Mirage.h"
#include "mt_random.h"

using namespace optix;

#ifndef JITTER
#define JITTER false
#endif


optix::float3 Mirage::shade(const Ray& r, HitInfo& hit, bool emit) const {
    if (hit.trace_depth >= max_depth)
        return make_float3(0.0f);

    float R;
    Ray reflected, refracted;
    HitInfo hit_reflected, hit_refracted;
    tracer->trace_reflected(r, hit, reflected, hit_reflected);
    tracer->trace_refracted(r, hit, refracted, hit_refracted, R);

    
    /*
    * Kind of weird expression to make sure that the mirage effect is only applied to the hot asphalt
    */
    if (hit.position.z - hit.position.x * 0.26 > 17 || hit.position.z - hit.position.x * 0.2 < -53 || hit.position.z * 0.2 + hit.position.x > 20)
    {
        return tracer->get_background(r.direction);
    }

    /*
    * Doing Russian roulette to decide whether we use reflection
    * or refraction.
    */
    if (R == 1.0f) {
        return shade_new_ray(reflected, hit_reflected);
    }
    else {
        /*
        * Couldn't get jitter to work... :(
        */
        if (JITTER) {
            optix::float3 const jitter_ray = make_float3(mt_random(), mt_random(), mt_random());
            refracted.direction = normalize(refracted.direction + jitter_ray);

        }

        return shade_new_ray(refracted, hit_refracted);
    }

}
