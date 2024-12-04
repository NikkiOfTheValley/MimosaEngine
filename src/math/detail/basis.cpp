#include <math/basis.h>
#include <math/mat4x4.h>
#include <physics/types/assert.h>

namespace math
{
	Basis::Basis(mat4x4f matrix)
	{

	}

	Basis::Basis(vec3 xAxis, vec3 yAxis, vec3 zAxis)
	{

	}

	Basis::Basis(Quaternion quaternion)
	{

	}

	Basis Basis::FromScale(float scale) const
	{

	}

	Basis Basis::FromEuler(vec3 euler) const
	{

	}

	vec3 Basis::GetEuler() const
	{

	}

	Quaternion Basis::GetRotationQuaternion() const
	{

	}

	vec3 Basis::GetScale() const
	{

	}

	Basis Basis::Inverse() const
	{

	}

	Basis Basis::LookAt(vec3 target, vec3 up) const
	{

	}

	Basis Basis::Orthonormalized() const
	{

	}

	Basis Basis::Rotated(vec3 axis, float angle, bool local) const
	{

	}

	Basis Basis::Scaled(vec3 scale, bool local) const
	{

	}

	Basis Basis::Slerp(Basis to, float weight) const
	{

	}

	Basis Basis::Transposed() const
	{

	}

	Basis operator*(const Basis& lhs, const Basis& rhs)
	{

	}

	vec3 operator*(const Basis& lhs, const vec3& rhs)
	{

	}

	Basis operator*(const Basis& lhs, const float& rhs)
	{

	}

	Basis operator*(const Basis& lhs, const int& rhs)
	{

	}

	Basis operator/(const Basis& lhs, const float& rhs)
	{

	}

	Basis operator/(const Basis& lhs, const int& rhs)
	{

	}

	bool Basis::operator==(const Basis& rhs)
	{

	}

	vec3& Basis::operator[](const size_t i)
	{
		assert(i > 2, "Basis operator[] index out of bounds");
	}
}