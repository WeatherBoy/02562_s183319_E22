// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <optix_world.h>
#include "HitInfo.h"
#include "Volume.h"

using namespace optix;

float3 Volume::shade(const Ray& r, HitInfo& hit, bool emit) const {
    // If inside the volume, Find the direct transmission through the volume by using
    // the transmittance to modify the result from the Transparent shader.

    /* 
    * We find the product between the transmittance T_r and the shade,
    * as per the equation of page one of worksheet08.
    * */

    return Transparent::shade(r, hit, emit) * get_transmittance(hit);
}

float3 Volume::get_transmittance(const HitInfo& hit) const {

    if (hit.material) {
        // Compute and return the transmittance using the diffuse reflectance of the material.
        // Diffuse reflectance rho_d does not make sense for a specular material, so we can use 
        // this material property as an absorption coefficient. Since absorption has an effect
        // opposite that of reflection, using 1/rho_d-1 makes it more intuitive for the user.
        float3 rho_d = make_float3(hit.material->diffuse[0], hit.material->diffuse[1], hit.material->diffuse[2]);

        /*
        * New epsilon every single time!
        * 
        * Finding the max of between rho_d and my value of epsilon, this is possible using the fmaxf()
        * function.
        */
        float3 const epsilon3 = make_float3(0.000001f);

        rho_d = fmaxf(rho_d, epsilon3);

        /*
        * Finding the exponential function of the negative absorption coefficient
        * times the distance as shown on page one of worksheet 08.
        * (This is the transmittance).
        * 
        * We can use expf() to take the exponential of vectors.
        */
        float3 const absorp_coeff = 1 / rho_d - make_float3(1.0f);

        return expf(-absorp_coeff * hit.dist);
    }

    return make_float3(1.0f);
}
