#pragma once
#include "quaternion.h"

namespace math
{
	//! @brief The basis of a Transform3D
	class Basis
	{
	public:
		vec3 xAxis;
		vec3 yAxis;
		vec3 zAxis;

		Basis(mat4x4f matrix);

		Basis(vec3 xAxis, vec3 yAxis, vec3 zAxis);

		Basis(Quaternion quaternion);

		Basis FromScale(float scale) const;

		Basis FromEuler(vec3 euler) const;

		vec3 GetEuler() const;

		Quaternion GetRotationQuaternion() const;

		vec3 GetScale() const;

		Basis Inverse() const;

		Basis LookAt(vec3 target, vec3 up = vec3(0, 1, 0)) const;

		Basis Orthonormalized() const;

		Basis Rotated(vec3 axis, float angle, bool local) const;

		Basis Scaled(vec3 scale, bool local) const;

		Basis Slerp(Basis to, float weight) const;

		Basis Transposed() const;

		friend Basis operator*(const Basis& lhs, const Basis& rhs);

		friend vec3 operator*(const Basis& lhs, const vec3& rhs);

		friend Basis operator*(const Basis& lhs, const float& rhs);

		friend Basis operator*(const Basis& lhs, const int& rhs);

		friend Basis operator/(const Basis& lhs, const float& rhs);

		friend Basis operator/(const Basis& lhs, const int& rhs);

		bool operator==(const Basis& rhs);

		vec3& operator[](const size_t i);
	};
}