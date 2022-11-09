// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <iostream>
#include <optix_world.h>
#include "my_glut.h"
#include "../SOIL/SOIL.h"
#include "Texture.h"

using namespace std;
using namespace optix;

void Texture::load(const char* filename)
{
  SOIL_free_image_data(data);
  data = SOIL_load_image(filename, &width, &height, &channels, SOIL_LOAD_AUTO);
  if(!data)
  {
    cerr << "Error: Could not load texture image file." << endl;
    return;
  }
  int img_size = width*height;
  delete[] fdata;
  fdata = new float4[img_size];
  for(int i = 0; i < img_size; ++i)
    fdata[i] = look_up(i);
  tex_handle = SOIL_create_OGL_texture(data, width, height, channels, tex_handle, SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
  tex_target = GL_TEXTURE_2D;
}

void Texture::load(GLenum target, GLuint texture)
{
  glBindTexture(target, texture);
  glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);
  delete [] fdata;
  fdata = new float4[width*height];
  glGetTexImage(target, 0, GL_RGBA, GL_FLOAT, &fdata[0].x);
  tex_handle = texture;
  tex_target = target;
}

float4 Texture::sample_nearest(const float3& texcoord) const {
    if (!fdata) {
        return make_float4(0.0f);

    }
    

    // Implement texture look-up of nearest texel.
    //
    // Input:  texcoord      (texture coordinates: u = texcoord.x, v = texcoord.y)
    //
    // Return: texel color found at the given texture coordinates
    //
    // Relevant data fields that are available (see Texture.h)
    // fdata                 (flat array of texture data: texel colors in float4 format)
    // width, height         (texture resolution)
    //
    // Hint: Remember to revert the vertical axis when finding the index
    //       into fdata.


    /*
    * These are defined as per the slide Texture look-up
    * (Slide 7, Week 4)
    */
    float const s = texcoord.x - floor(texcoord.x);
    float const t = -texcoord.y - floor(-texcoord.y); // minus in front of y was the easiest way to reverse the im
    float const a = s * width;
    float const b = t * height;

    /*
    * Look at slide Texture Filtering (nearest neighbor filtering)
    * (Slide 8, week 4)
    */
    float const U = static_cast<int>(a + 0.5) % width;
    float const V = static_cast<int>(b + 0.5) % height;


    return fdata[static_cast<int>(U + V * width)];
}

float4 Texture::sample_linear(const float3& texcoord) const {
    if (!fdata) {
        return make_float4(0.0f);

    }
    

    // Implement texture look-up which returns the bilinear interpolation of
    // the four nearest texel neighbors.
    //
    // Input:  texcoord      (texture coordinates: u = texcoord.x, v = texcoord.y)
    //
    // Return: texel color found at the given texture coordinates after
    //         bilinear interpolation
    //
    // Relevant data fields that are available (see Texture.h)
    // fdata                 (flat array of texture data: texel colors in float4 format)
    // width, height         (texture resolution)
    //
    // Hint: Use three lerp operations (or one bilerp) to perform the
    //       bilinear interpolation.

    /*
    * These are defined as per the slide Texture look-up
    * (Slide 7, Week 4)
    */
    float const s = texcoord.x - floor(texcoord.x);
    float const t = -texcoord.y - floor(-texcoord.y); // minus in front of y was the easiest way to reverse the im
    float const a = s * width;
    float const b = t * height;

    /*
    * Look at slide Texture Filtering (bilinear filtering)
    * (Slide 8, week 4)
    */
    int const U = static_cast<int>(a);
    int const V = static_cast<int>(b);
    float const c_1 = a - U;
    float const c_2 = b - V;

    /*
    * With those variables defined I now need to find the four corners closest to
    * the point that I am looking at.
    * 
    * We use the modulo operator to make sure that we handle edge cases properly.
    */
    int const U0 = U % width;
    int const U1 = (U + 1) % width;
    int const V0 = V % height;
    int const V1 = (V + 1) % height;
    float4 const x00 = fdata[U0 + V0 * width];
    float4 const x10 = fdata[U1 + V0 * width];
    float4 const x01 = fdata[U0 + V1 * width];
    float4 const x11 = fdata[U1 + V1 * width];



    return bilerp(x00, x10, x01, x11, c_1, c_2);
    //return sample_nearest(texcoord);
}

float4 Texture::look_up(unsigned int idx) const
{
  idx *= channels;
  switch(channels)
  {
  case 1:
  {
    float v = convert(data[idx]);
    return make_float4(v, v, v, 1.0f);
  }
  case 2:
    return make_float4(convert(data[idx]), convert(data[idx]), convert(data[idx]), convert(data[idx + 1]));
  case 3:
    return make_float4(convert(data[idx]), convert(data[idx + 1]), convert(data[idx + 2]), 1.0f);
  case 4:
    return make_float4(convert(data[idx]), convert(data[idx + 1]), convert(data[idx + 2]), convert(data[idx + 3]));
  }
  return make_float4(0.0f);
}

float Texture::convert(unsigned char c) const
{
  return (c + 0.5f)/256.0f;
}
