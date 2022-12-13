#include <iostream>
#include <optix_world.h>
#include "mt_random.h"
#include "PathTracer.h"
using namespace std;

using namespace optix;

/*
* Defined these constants myself for the mirage project
*/
#ifndef ALPHA
#define ALPHA 0.43421623968736434
#endif

#ifndef ETA0_SQR
#define ETA0_SQR 1.0004660542889998
#endif

#ifndef ETA1_SQR
#define ETA1_SQR 0.21013055999999997
#endif

#ifndef EPSILON
#define EPSILON 0.001
#endif

void PathTracer::update_pixel(unsigned int x, unsigned int y, float sample_number, float3& L) const
{
  float2 ip_coords = make_float2(x + mt_random(), y + mt_random())*win_to_ip + lower_left;
  Ray r = scene->get_camera()->get_ray(ip_coords);

  float const ior = sqrt(ETA0_SQR + ETA1_SQR * (1 - exp(-ALPHA *  r.origin.y ) ) );
;

  HitInfo hit;
  hit.ray_ior = ior;

  L *= sample_number;
  if(trace_to_closest(r, hit))
  {
    const Shader* shader = get_shader(hit);
    if(shader)
      L += shader->shade(r, hit);
  }
  else
    L += get_background(r.direction);
  L /= sample_number + 1.0f;
}

