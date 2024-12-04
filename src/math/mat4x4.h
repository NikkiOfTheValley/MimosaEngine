#pragma once
#include "vec.h"
#include "math/conversion.h"
#include <numbers>
#include <string>
#include <sstream>

namespace math
{
	// 4x4 matrix of floats
// Data organization is row-major (i.e. [x][y])
	class mat4x4f
	{
	public:
		__declspec(align(16))
		float data[4][4] = {
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0} };

		mat4x4f();

		// Set the matrix to an identity matrix
		void identity();

		// Set the matrix to a perspective matrix
		void perspective(const float aspectRatio, const float nearClip, const float farClip, const float fov);

		// Set the matrix to a 2D orthographic matrix
		void ortho2D(const unsigned int width, const unsigned int height, const float nearClip, const float farClip);

		// Set the matrix to a camera matrix
		void lookAt(const vec3 eye, const vec3 center, const vec3 up);

		// Set the matrix to a translation matrix
		void translate(vec3 offset);

		/*
		   Set the matrix to a rotation matrix
		   Rotation matrix from https://en.wikipedia.org/wiki/Rotation_matrix
		   The rotation vector is a normalized vector that represents the (improper)
		   Euler angles about X, Y, and Z
		*/
		void rotate(vec3 rotation);

		/*!
		 * @brief Sets the matrix to a rotation matrix about `axis` with angle `angle`
		 * @param axis The axis to rotate around
		 * @param angle The angle to rotate to
		 */
		void rotate(vec3 axis, float angle);

		mat4x4f operator*=(const mat4x4f& rhs);

		friend mat4x4f operator*(const mat4x4f& lhs, const mat4x4f& rhs);

		bool operator==(const mat4x4f& rhs);

		operator std::string() const;
	};

	// 4x4 matrix of ints
	class mat4x4i
	{
		int data[4][4] = {
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0} };

		mat4x4i();

		// Sets the matrix to an identity matrix
		void identity();
	};
}