#pragma once
#include "vec3.h"
#include "vec2.h"

namespace math
{
	//! @brief Computes the square root
	vec3 sqrt(vec3 v);

	//! @brief Computes the inverse square root
	vec3 invsqrt(vec3 v);

	//! @brief Computes the inverse square root
	float invsqrt(float v);

	//! @brief Computes the cross product of two vectors
	vec3 cross(vec3 a, vec3 b);

	//! @brief Computes the dot product of two vectors
	float dot(vec3 a, vec3 b);

	//! @brief Computes the dot product of two vectors
	float dot(vec2 a, vec2 b);

	//! @brief Normalizes a vector
	vec3 normalize(vec3 v);

	//! @brief Normalizes a vector
	vec2 normalize(vec2 v);

	//! @brief Checks if a vector is NaN
	bool is_nan(vec3 v);

	//! @brief Checks if a vector is NaN
	bool is_nan(vec2 v);

	//! @brief Computes the magnitude (aka length) of a vector
	float magnitude(vec3 v);
	
	//! @brief Computes the magnitude (aka length) of a vector
	float magnitude(vec2 v);

	//! @brief Computes the angle between two vectors in radians
	float angleBetween(vec3 a, vec3 b);

	//! @brief Computes the angle between two vectors in radians. Optimized for normalized vectors
	float angleBetweenNormalized(vec3 a, vec3 b);

	//! @brief Computes the distance^2 between two points
	float squaredDist(vec3 a, vec3 b);

	//! @brief Computes the distance^2 between two points
	float squaredDist(vec2 a, vec2 b);

	//! @brief Computes the distance between two points
	float dist(vec3 a, vec3 b);

	//! @brief Computes the distance between two points
	float dist(vec2 a, vec2 b);

	//! @brief Linearly interpolates between two vectors
	vec3 lerp(vec3 a, vec3 b, float weight);

	//! @brief Linearly interpolates between two vectors
	vec2 lerp(vec2 a, vec2 b, float weight);

	//! @brief Computes the triple product
	vec3 tripleProduct(vec3 a, vec3 b, vec3 c);

	//! @brief Returns the barycentric coordinates of `point` for the triangle a, b, c
	vec3 barycentricTriangle(vec3 a, vec3 b, vec3 c, vec3 point);

	/*! @brief Returns the point p projected onto the triangle a, b, c.
	* Assumes the point is within the triangle for performance reasons.
	*/
	vec3 projectPointOntoTri(vec3 a, vec3 b, vec3 c, vec3 p);
}