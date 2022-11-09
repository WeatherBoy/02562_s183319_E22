// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#ifndef FRESNEL_H
#define FRESNEL_H

inline float fresnel_r_s(float cos_theta1, float cos_theta2, float ior1, float ior2) {
	/*
	* My own definitions (based on what Jakob said):
	* 
	* ior1 := index of refraction (ray reaches - n_i)
	* ior2 := index of refraction (ray transmits - n_t)
	* cos_theta1 := cos(theta_i)
	* cos_theta2 := cos(theta_t)
	*/
	// Compute the perpendicularly polarized component of the Fresnel reflectance

	return (ior1 * cos_theta1 - ior2 * cos_theta2) / (ior1 * cos_theta1 + ior2 * cos_theta2);
}

inline float fresnel_r_p(float cos_theta1, float cos_theta2, float ior1, float ior2) {
	/*
	* See definitions in fresnel_r_s().
	*/
	// Compute the parallelly polarized component of the Fresnel reflectance

	return (ior2 * cos_theta1 - ior1 * cos_theta2) / (ior2 * cos_theta1 + ior1 * cos_theta2);
}

inline float fresnel_R(float cos_theta1, float cos_theta2, float ior1, float ior2) {
	/*
	* See definitions in fresnel_r_s().
	*/
	// Compute the Fresnel reflectance using fresnel_r_s(...) and fresnel_r_p(...)
	float const r_s = fresnel_r_s(cos_theta1, cos_theta2, ior1, ior2);
	float const r_p = fresnel_r_p(cos_theta1, cos_theta2, ior1, ior2);

	return 0.5 * (r_s * r_s + r_p * r_p);
}

#endif // FRESNEL_H
