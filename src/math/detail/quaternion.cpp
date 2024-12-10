#include <math/quaternion.h>
#include <physics/types/assert.h>
#include <math/basis.h>

#pragma warning(disable : 4100)

namespace math
{
	Quaternion::Quaternion(vec3 arcFrom, vec3 arcTo)
	{
		
	}

	Quaternion::Quaternion(vec3 axis, float angle)
	{

	}

	Quaternion::Quaternion(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	float Quaternion::AngleTo(Quaternion to) const
	{
		float d = dot(to);
		return acos(d * d * 2 - 1);
	}

	float Quaternion::dot(Quaternion q) const
	{
		return x * q.x + y * q.y + z * q.z + w * q.w;
	}

	Quaternion Quaternion::exp() const
	{

	}

	Quaternion Quaternion::log() const
	{

	}

	Quaternion Quaternion::inverse() const
	{

	}

	float Quaternion::length_squared() const
	{
		return dot(*this);
	}

	float Quaternion::length() const
	{
		return std::sqrt(length_squared());
	}

	Quaternion Quaternion::FromEuler(vec3 euler)
	{

	}

	vec3 Quaternion::GetAxis() const
	{
		if (abs(w) > 1 - flt_cmp_epsilon)
			return vec3(x, y, z);

		float r = 1.f / std::sqrt(1 - w * w);
		return vec3(x * r, y * r, z * r);
	}

	float Quaternion::GetAngle() const
	{
		return 2 * acos(w);
	}

	vec3 Quaternion::GetEuler() const
	{
		assert(IsNormalized(), "Quaternion must be normalized");

		return Basis(*this).GetEuler();
	}

	bool Quaternion::IsEqualApprox(Quaternion q) const
	{
		bool isNotEqual = false;

		isNotEqual |= abs(x - q.x) > flt_cmp_epsilon;
		isNotEqual |= abs(y - q.y) > flt_cmp_epsilon;
		isNotEqual |= abs(z - q.z) > flt_cmp_epsilon;
		isNotEqual |= abs(w - q.w) > flt_cmp_epsilon;

		return !isNotEqual;
	}

	bool Quaternion::IsNormalized() const
	{
		return length() <= 1.f;
	}

	Quaternion Quaternion::Normalized() const
	{
		return *this;
	}

	Quaternion Quaternion::slerp(Quaternion to, float weight) const
	{

	}

	Quaternion Quaternion::slerpni(Quaternion to, float weight) const
	{

	}

	Quaternion Quaternion::SphericalCubicInterpolate(Quaternion b, Quaternion preA, Quaternion postB, float weight) const
	{

	}

	Quaternion Quaternion::SphericalCubicInterpolateInTime(Quaternion b, Quaternion preA, Quaternion postB, float bTime, float preATime, float postBTime, float weight) const
	{

	}
}