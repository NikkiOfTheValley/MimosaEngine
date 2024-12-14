#pragma once
#include <math/vec3.h>
#include <math/mat4x4.h>
#include <math/mat3x3.h>

namespace math
{
	class Quaternion;

	//! @brief The basis of a Transform3D
	class Basis
	{
	public:
		vec3 xAxis = vec3(1, 0, 0);
		vec3 yAxis = vec3(0, 1, 0);
		vec3 zAxis = vec3(0, 0, 1);

		Basis() {};

		Basis(mat4x4f matrix);

		Basis(mat3x3f matrix);

		Basis(vec3 xAxis, vec3 yAxis, vec3 zAxis);

		Basis(Quaternion quaternion);

		Basis Identity() const;

		float Determinant() const;

		Basis FromScale(float scale) const;

		Basis FromEuler(vec3 euler) const;

		vec3 GetEuler() const;

		Quaternion GetRotationQuaternion() const;

		vec3 GetScaleAbs() const;

		vec3 GetScaleGlobal() const;

		vec3 GetScale() const;

		Basis Inverse() const;

		Basis Orthonormalized() const;

		Basis Rotated(vec3 axis, float angle, bool local) const;

		Basis Scaled(vec3 scale, bool local) const;

		Basis Lerp(Basis to, float weight) const;

		Basis Slerp(Basis to, float weight) const;

		Basis LookAt(const vec3& target, const vec3& up) const;

		Basis Transposed() const;

		mat3x3f Matrix() const;

		friend Basis operator*(const Basis& lhs, const Basis& rhs);

		friend vec3 operator*(const Basis& lhs, const vec3& rhs);

		friend Basis operator*(const Basis& lhs, const float& rhs);

		friend Basis operator*(const Basis& lhs, const int& rhs);

		friend Basis operator/(const Basis& lhs, const float& rhs);

		friend Basis operator/(const Basis& lhs, const int& rhs);

		bool operator==(const Basis& rhs);

		vec3& operator[](const size_t i);
		const vec3& operator[](const size_t i) const;
	};
}