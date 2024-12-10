#pragma once
#include <string>
#include <sstream>
#include "core/log.h"
#include "core/globals.h"

namespace math
{
	/*
	Predefine classes so no circular dependency is
	required between headers
	*/

	class mat4x4f;
	class mat3x3f;
	class Transform3D;

	struct vec3
	{
	public:
		// The components of the vector
		float x;
		float y;
		float z;

		// Default constructor, initilizes the components to 0
		vec3();

		// Initilizes the components to x, y, & z
		vec3(float x, float y, float z);

		// Integer version of the float constructor
		vec3(int x, int y, int z);

		// Unsigned integer version of the float constructor
		vec3(unsigned int x, unsigned int y, unsigned int z);

		void operator+=(const vec3& rhs);

		void operator+=(const float rhs);

		void operator-=(const vec3& rhs);

		void operator-=(const float rhs);

		void operator*=(const vec3& rhs);

		void operator*=(const float rhs);

		void operator*=(const mat4x4f rhs);

		void operator*=(const mat3x3f rhs);

		void operator*=(const Transform3D rhs);

		void operator/=(const vec3& rhs);

		void operator/=(const float rhs);


		friend vec3 operator+(const vec3& lhs, const vec3& rhs);

		friend vec3 operator+(const vec3& lhs, const float rhs);

		friend vec3 operator-(const vec3& lhs, const vec3& rhs);

		friend vec3 operator-(const vec3& lhs, const float rhs);

		friend vec3 operator*(const vec3& lhs, const vec3& rhs);

		friend vec3 operator*(const vec3& lhs, const float rhs);

		friend vec3 operator*(const vec3& lhs, const mat4x4f rhs);

		friend vec3 operator*(const vec3& lhs, const mat3x3f rhs);

		friend vec3 operator*(const vec3& lhs, const Transform3D rhs);

		friend vec3 operator/(const vec3& lhs, const vec3& rhs);

		friend vec3 operator/(const vec3& lhs, const float rhs);


		friend vec3 operator-(const vec3& lhs);


		bool operator==(const vec3& rhs);
		bool operator!=(const vec3& rhs);
		float& operator[](const size_t i);
		const float& operator[](const size_t i) const;

		operator std::string() const;
	};
}