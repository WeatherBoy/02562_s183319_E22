// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <iostream>
#include <optix_world.h>
#include "mt_random.h"
#include "Shader.h"
#include "HitInfo.h"
#include "RayCaster.h"

using namespace std;
using namespace optix;

float3 RayCaster::compute_pixel(unsigned int x, unsigned int y) const {

    /*
    * I was told that I should return red, just so that I can see something
    * happening.
    * The "f" behind the number tells the compiler that this should be a 
    * floating-point instead of a double (doubles are bigger).
    */
    // float3 result = make_float3(1.0f, 0.0f, 0.0f);

    // Use the scene and its camera
    // to cast a ray that computes the color of the pixel at index (x, y).
    //
    // Input:  x, y        (pixel index)
    //
    // Return: Result of tracing a ray through the pixel at index (x, y).
    //
    // Relevant data fields that are available (see RayCaster.h and Scene.h)
    // win_to_ip           (pixel size (width, height) in the image plane)
    // lower_left          (lower left corner of the film in the image plane)
    // scene               (scene with access to the closest_hit function)
    // scene->get_camera() (camera in the scene)
    //
    // Hints: (a) Use the function get_shader(...) to get the shader of the
    //            intersected material after the ray has been traced.
    //        (b) Use get_background(...) if the ray does not hit anything.

    /*
    * First we transform our provided coordinates into a float2.
    * Then we get a camera from something we call a "scene".
    * This camera is what we have been working on for the most part,
    * and as such it has the functions we desire.
    * We get a ray "some_ray" which we don't use for anything yet and then
    * a ray_direction "ray_dir" which is of type float3 so that we
    * can multiply it by 0.5 and add 0.5 to it as per the exercise.
    * float3 ray_dir = cam->get_ray_dir(coords);
    * float3 result = ray_dir * 0.5 + 0.5;
    */
    // int number_hits = 0;
    //  float2 const coords = make_float2(x, y);

    Camera* cam = scene->get_camera();

    /*
    * Times the vector with "win_to_ip" to go from windows coordinates to image pixels,
    * then we add "lower_left" as we would otherwise have shifted the entire scene.
    */
    float2 coords = make_float2(x, y) * win_to_ip + lower_left;

    Ray some_ray{};

    /*
    * I'll try and create fewer uninstantiated objects, because that is what Jakob
    * said and he is big intellect!
    */
    float3 result = make_float3(0.0f, 0.0f, 0.0f);
    

    /*
    * Create a blank instance of the HitInfo class, which can be provided information
    * from the intersect functions of our objects.
    * (All it will ever know is what it will learn in the intersect functions).
    */
    HitInfo hit{};

    
    /*
    * To cast the ray through each sub-pixelm we run a double for-loop over our subdivision.
    * Here we find the ray at each of the subdivisions by adding 'jitter' to the coords,
    * We then find the cumulative result of the pixel, which is then divided by the
    * amount of subpixels (subdivs * subdivs) to get a mean of our pixel over the subpixels.
    */
    for (unsigned int i = 0; i < subdivs; ++i) {
        for (unsigned int j = 0; j < subdivs; ++j) {
            some_ray = cam->get_ray(coords + jitter[i * subdivs + j]);

            if (scene->closest_hit(some_ray, hit)) {
                /*
                * We get a shader by using the scenes get_shader(). This shader has a function
                * called shade(), which will result in a float3, which will tell us what to
                * colour our pixel.
                */
                result += scene->get_shader(hit)->shade(some_ray, hit);

            }
            else {
                /*
                * If nothing was hit - we will just draw the background.
                */
                result += get_background();

            }
        }
    }
    
    
    

    /*
    * Dividing by subdivs * subdivs to get a mean result, instead of a wildly disproportionate
    * sum.
    */
    return result / (subdivs * subdivs);
}

float3 RayCaster::get_background(const float3& dir) const
{ 
  if(!sphere_tex)
    return background;
  return make_float3(sphere_tex->sample_linear(dir));
}

void RayCaster::increment_pixel_subdivs()
{
  ++subdivs;
  compute_jitters();
  cout << "Rays per pixel: " << subdivs*subdivs << endl;
}

void RayCaster::decrement_pixel_subdivs()
{
  if(subdivs > 1)
  {
    --subdivs;
    compute_jitters();
  }
  cout << "Rays per pixel: " << subdivs*subdivs << endl;
}

void RayCaster::compute_jitters() {
    float aspect = width/static_cast<float>(height);
    win_to_ip.x = win_to_ip.y = 1.0f/static_cast<float>(height);
    lower_left = (win_to_ip - make_float2(aspect, 1.0f))*0.5f;
    step = win_to_ip/static_cast<float>(subdivs);

    jitter.resize(subdivs*subdivs);
    for (unsigned int i = 0; i < subdivs; ++i) {
        for (unsigned int j = 0; j < subdivs; ++j) {
            jitter[i * subdivs + j] = make_float2(safe_mt_random() + j, safe_mt_random() + i) * step - win_to_ip * 0.5f;

        }

    }
         
}
