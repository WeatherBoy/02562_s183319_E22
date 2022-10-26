#include <iostream>
#include <optix_world.h>
#include "my_glut.h"
#include "Camera.h"

using namespace optix;

void Camera::set(const float3& eye_point, const float3& view_point, const float3& up_vector, float camera_constant) {
	eye = eye_point;
	lookat = view_point;
	up = up_vector;
	cam_const = camera_constant;

	// Compute camera coordinate frame (image plane normal and basis).
	//
	// Relevant data fields that are available (see Camera.h)
	// ip_normal  (the viewing direction and the 3rd basis vector, v)
	// ip_xaxis   (the 1st basis vector, b_1)
	// ip_yaxis   (the 2nd basis vector, b_2)
	//
	// Hint: The OptiX math library has a function normalize(v) which returns
	//       the vector v normalized and a function cross(v, w) which returns
	//       the cross product of the vectors v and w.

	/*
	* I use the formulas of "Ray generation" from the slides from week 1
	*/
	ip_normal = (lookat - eye) / sqrt(dot(lookat - eye, lookat - eye));

	/*
	* First I save the cross product, because I have to use it thrice.
	* All the other variables that don't need types are supposedly fields
	* of the "camera.h" file.
	*/
	float3 const cross_normal_up = cross(ip_normal, up);
	ip_xaxis = cross_normal_up / sqrt(dot(cross_normal_up, cross_normal_up));
	ip_yaxis = cross(ip_xaxis, ip_normal);



	// Assume that the height and the width of the film is 1.
	// With this information, use the pinhole camera model to compute
	// the field of view (fov) in degrees.
	//
	// Relevant data fields that are available (see Camera.h)
	// camera_constant  (the camera constant, d)
	// fov              (the vertical field of view of the camera, phi)
	//
	// Hints: (a) The inverse tangent function is called atan(x).
	//        (b) The OptiX math library has a constant for 1/pi called M_1_PIf.
	//        (c) The following constant is the field of view that you should 
	//            get with the default scene (if you set a breakpoint and run
	//            in Debug mode, you can use it to check your code).
	// fov = 53.13f;

	/*
	* We tried to solve for phi in the equation stated on slide 18
	* of the first week.
	* Then we have to convert it from radians back to degrees,
	* which is done by 180/pi (don't know why we use degress though..!?)
	*/
	fov = 2 * atan(1 / (2 * cam_const)) * 180 * M_1_PIf;


}

/// Get direction of viewing ray from image coords.
float3 Camera::get_ray_dir(const float2& coords) const {
	// Given the image plane coordinates, compute the normalized ray
	// direction by a change of basis.

	/*
	* We calculate q as described in Ray generation (also slides week 1, more
	* specifically slide 20).
	*/
	float3 const q = ip_xaxis * coords.x + ip_yaxis * coords.y + ip_normal * cam_const;

	/*
	* Then we calculate the ray direction by using q, again as described by the equation
	* from slide 20 (Ray generration).
	*/
	float3 const ray_direction = q / sqrt(dot(q, q));

	return ray_direction;


}

/// Return the ray corresponding to a set of image coords
Ray Camera::get_ray(const float2& coords) const {
	// Use the function get_ray_dir(...) to construct a ray starting at
	// the eye and going through the position in the image plane given
	// by the image plane coordinates received as argument.
	//
	// Hint: You can set the ray type to 0 as the framework currently
	//       does not use different ray types.

	/*
	* Using that ray_direction as specfied. I assume that the cords
	* received in this function should parsed on through as 
	* the type matches.
	*/
	float3 const ray_dir = get_ray_dir(coords);

	/*
	* Then we generate a Ray. The constructor for this class can be
	* seen on the slide "Rays in theory and in practice" (slides 21).
	*/
	Ray const resulting_ray = Ray(eye, ray_dir, 0, 0);

	return resulting_ray;
}

// OpenGL

void Camera::glSetPerspective(unsigned int width, unsigned int height) const
{
  GLdouble aspect = width/static_cast<float>(height);    

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, aspect, cam_const*NEAR_PLANE, FAR_PLANE);

  glMatrixMode(GL_MODELVIEW);
}

void Camera::glSetCamera() const
{
  gluLookAt(eye.x,   eye.y,   eye.z, 
	          lookat.x, lookat.y, lookat.z, 
	          up.x,    up.y,    up.z);
}

