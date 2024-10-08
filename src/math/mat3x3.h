#pragma once
#include "vec.h"
#include "math/conversion.h"
#include <numbers>
#include <string>
#include <sstream>

namespace math
{
	// 3x3 matrix of floats
	// Data organization is row-major (i.e. [x][y])
	class mat3x3f
	{
	public:
		float data[3][3] = {
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0} };

		mat3x3f();

		// Set the matrix to an identity matrix
		void identity();

		// Set the matrix to a translation matrix
		void translate(vec3 offset);

		/*
		   Set the matrix to a rotation matrix
		   Rotation matrix from https://en.wikipedia.org/wiki/Rotation_matrix
		   The rotation vector is a normalized vector that represents the (improper)
		   Euler angles about X, Y, and Z
		*/
		void rotate(vec3 rotation);

		float determinant();

		// Computes the inverse matrix of the matrix it is called on, then returns it
		mat3x3f inverse();

		operator std::string() const;

	private:
		float determinantHelper(int columnWidthX);
	};
}