#pragma once
#include "../mat3x3.h"

namespace math
{
	mat3x3f::mat3x3f() { };

	void mat3x3f::identity()
	{
		data[0][0] = 1;
		data[1][1] = 1;
		data[2][2] = 1;
	}

	void mat3x3f::translate(vec3 offset)
	{
		// First initilize the matrix as an identity matrix
		identity();

		// Then add in the translation offset
		data[2][0] = offset.x;
		data[2][1] = offset.y;
		data[2][2] = offset.z;
	}

	void mat3x3f::rotate(vec3 rotation)
	{
		// Rotation matrix from https://en.wikipedia.org/wiki/Rotation_matrix

		float rotX = rotation.x;
		float rotY = rotation.y;
		float rotZ = rotation.z;

		data[0][0] = cos(rotY) * cos(rotZ);
		data[1][0] = cos(rotY) * sin(rotZ);
		data[2][0] = -sin(rotY);

		data[0][1] = sin(rotX) * sin(rotY) * cos(rotZ) - cos(rotX) * sin(rotZ);
		data[1][1] = sin(rotX) * sin(rotY) * sin(rotZ) + cos(rotX) * cos(rotZ);
		data[2][1] = sin(rotX) * cos(rotY);

		data[0][2] = cos(rotX) * sin(rotY) * cos(rotZ) + sin(rotX) * sin(rotZ);
		data[1][2] = cos(rotX) * sin(rotY) * sin(rotZ) - sin(rotX) * cos(rotZ);
		data[2][2] = cos(rotX) * cos(rotY);
	}

	float mat3x3f::determinantHelper(int columnWidthX)
	{
		int x1 = columnWidthX == 0 ? 1 : 0;  /* always either 0 or 1 */
		int x2 = columnWidthX == 2 ? 1 : 2;  /* always either 1 or 2 */

		return (data[1][x1] * data[2][x2])
			- (data[1][x2] * data[2][x1]);
	}

	float mat3x3f::determinant()
	{
		return (data[0][0] * determinantHelper(0))
			- (data[0][1] * determinantHelper(1))
			+ (data[0][2] * determinantHelper(2));
	}

	mat3x3f mat3x3f::inverse()
	{
		float inverse_determinant = 1 / determinant();

		mat3x3f result;

		result.data[0][0] = (data[1][1] * data[2][2] - data[2][1] * data[1][2]) * inverse_determinant;
		result.data[0][1] = (data[0][2] * data[2][1] - data[0][1] * data[2][2]) * inverse_determinant;
		result.data[0][2] = (data[0][1] * data[1][2] - data[0][2] * data[1][1]) * inverse_determinant;
		result.data[1][0] = (data[1][2] * data[2][0] - data[1][0] * data[2][2]) * inverse_determinant;
		result.data[1][1] = (data[0][0] * data[2][2] - data[0][2] * data[2][0]) * inverse_determinant;
		result.data[1][2] = (data[1][0] * data[0][2] - data[0][0] * data[1][2]) * inverse_determinant;
		result.data[2][0] = (data[1][0] * data[2][1] - data[2][0] * data[1][1]) * inverse_determinant;
		result.data[2][1] = (data[2][0] * data[0][1] - data[0][0] * data[2][1]) * inverse_determinant;
		result.data[2][2] = (data[0][0] * data[1][1] - data[1][0] * data[0][1]) * inverse_determinant;

		return result;
	}

	mat3x3f::operator std::string() const {
		std::stringstream resultString;
		resultString <<
			"{" << "{" << data[0][0] << ", " << data[0][1] << ", " << data[0][2] << "},\n" <<
			" {" << data[1][0] << ", " << data[1][1] << ", " << data[1][2] << "},\n" <<
			" {" << data[2][0] << ", " << data[2][1] << ", " << data[2][2] << "}}\n";

		return resultString.str();
	}
}