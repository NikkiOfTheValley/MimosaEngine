#pragma once
#include "../mat4x4.h"

namespace math
{
	mat4x4f::mat4x4f() { };

	void mat4x4f::identity()
	{
		data[0][0] = 1;
		data[1][1] = 1;
		data[2][2] = 1;
		data[3][3] = 1;
	}

	void mat4x4f::perspective(const float aspectRatio, const float nearClip, const float farClip, const float fov)
	{
		if (abs(aspectRatio - std::numeric_limits<float>::epsilon()) < 0)
			logger.err("I have no idea what this means. (in mat4x4f::perspective())");

		const double scale = tan(conv::deg_to_rad(fov) / 2);

		data[0][0] = (float)1 / (float)(aspectRatio * scale);
		data[1][1] = (float)1 / (float)(scale);
		data[2][2] = -(farClip + nearClip) / (farClip - nearClip);
		data[2][3] = -1;
		data[3][2] = -(2 * farClip * nearClip) / (farClip - nearClip);
	}

	void mat4x4f::ortho2D(const unsigned int w, const unsigned int h, const float nearClip, const float farClip)
	{
		unsigned int xMax = w - 1;
		unsigned int yMax = h - 1;

		data[0][0] = 2.f / xMax;
		data[1][1] = -2.f / yMax;
		data[2][2] = 2.f / (farClip - nearClip);
		data[3][2] = (nearClip + farClip) / (nearClip - farClip);

		data[3][0] = -1.f;
		data[3][1] = 1.f;
		data[3][3] = 1.f;
	}

	void mat4x4f::lookAt(const vec3 eye, const vec3 center, const vec3 up)
	{
		const vec3 f = vec3(normalize(center - eye));
		const vec3 s(normalize(cross(f, up)));
		const vec3 u(cross(s, f));

		identity();

		data[0][0] = s.x;
		data[1][0] = s.y;
		data[2][0] = s.z;
		data[0][1] = u.x;
		data[1][1] = u.y;
		data[2][1] = u.z;
		data[0][2] = -f.x;
		data[1][2] = -f.y;
		data[2][2] = -f.z;
		data[3][0] = -dot(s, eye);
		data[3][1] = -dot(u, eye);
		data[3][2] = dot(f, eye);
	}

	void mat4x4f::translate(vec3 offset)
	{
		// First initilize the matrix as an identity matrix
		identity();

		// Then add in the translation offset
		data[3][0] = offset.x;
		data[3][1] = offset.y;
		data[3][2] = offset.z;
	}

	void mat4x4f::rotate(vec3 rotation)
	{
		// Rotation matrix from https://en.wikipedia.org/wiki/Rotation_matrix

		// First initilize the matrix as an identity matrix so this matrix acts
		// like the original 3x3 matrix when multiplying a vector by this matrix
		identity();

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


	mat4x4f::operator std::string() const {
		std::stringstream resultString;
		resultString <<
			"{" << "{" << data[0][0] << ", " << data[0][1] << ", " << data[0][2] << ", " << data[0][3] << "},\n" <<
			" {" << data[1][0] << ", " << data[1][1] << ", " << data[1][2] << ", " << data[1][3] << "},\n" <<
			" {" << data[2][0] << ", " << data[2][1] << ", " << data[2][2] << ", " << data[2][3] << "},\n" <<
			" {" << data[3][0] << ", " << data[3][1] << ", " << data[3][2] << ", " << data[3][3] << "}}\n";

		return resultString.str();
	}


	mat4x4i::mat4x4i() {};

	void mat4x4i::identity()
	{
		data[0][0] = 1;
		data[1][1] = 1;
		data[2][2] = 1;
		data[3][3] = 1;
	}
}