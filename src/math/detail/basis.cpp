#include <math/basis.h>
#include <math/quaternion.h>
#include <math/mat4x4.h>
#include <math/mat3x3.h>
#include <physics/types/assert.h>
#include <math/math_util.h>

namespace math
{
	Basis::Basis(mat4x4f matrix)
	{
		xAxis[0] = matrix.data[0][0];
		xAxis[1] = matrix.data[0][1];
		xAxis[2] = matrix.data[0][2];

		yAxis[0] = matrix.data[1][0];
		yAxis[1] = matrix.data[1][1];
		yAxis[2] = matrix.data[1][2];

		zAxis[0] = matrix.data[2][0];
		zAxis[1] = matrix.data[2][1];
		zAxis[2] = matrix.data[2][2];
	}

	Basis::Basis(mat3x3f matrix)
	{
		xAxis[0] = matrix.data[0][0];
		xAxis[1] = matrix.data[0][1];
		xAxis[2] = matrix.data[0][2];

		yAxis[0] = matrix.data[1][0];
		yAxis[1] = matrix.data[1][1];
		yAxis[2] = matrix.data[1][2];

		zAxis[0] = matrix.data[2][0];
		zAxis[1] = matrix.data[2][1];
		zAxis[2] = matrix.data[2][2];
	}

	Basis::Basis(vec3 xAxis, vec3 yAxis, vec3 zAxis)
	{
		this->xAxis = xAxis;
		this->yAxis = yAxis;
		this->zAxis = zAxis;
	}

	Basis::Basis(Quaternion /*quaternion*/)
	{
		logger.warn("Constructing a Basis from a Quaternion is not implemented!");
	}

	Basis Basis::Identity() const
	{
		return Basis(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
	}

	float Basis::Determinant() const
	{
		const Basis& basis = *this;

		return basis[0][0] * (basis[1][1] * basis[2][2] - basis[2][1] * basis[1][2]) -
			   basis[1][0] * (basis[0][1] * basis[2][2] - basis[2][1] * basis[0][2]) +
			   basis[2][0] * (basis[0][1] * basis[1][2] - basis[1][1] * basis[0][2]);
	}

	Basis Basis::FromScale(float scale) const
	{
		return Identity() * scale;
	}

	Basis Basis::FromEuler(vec3 euler) const
	{
		mat3x3f mat;
		mat.rotate(normalize(euler));

		return Basis(mat);
	}

	vec3 Basis::GetEuler() const
	{
		logger.warn("Basis::GetEuler() is not implemented!");
		return vec3();
	}

	Quaternion Basis::GetRotationQuaternion() const
	{
		logger.warn("Basis::GetRotationQuaternion() is not implemented!");
		return Quaternion();
	}

	vec3 Basis::GetScaleAbs() const
	{
		return vec3(
			magnitude(vec3(xAxis.x, yAxis.x, zAxis.x)),
			magnitude(vec3(xAxis.y, yAxis.y, zAxis.y)),
			magnitude(vec3(xAxis.z, yAxis.z, zAxis.z)));
	}

	vec3 Basis::GetScaleGlobal() const
	{
		float detSign = SIGN(Determinant());

		return vec3(magnitude(xAxis), magnitude(yAxis), magnitude(zAxis)) * detSign;
	}

	vec3 Basis::GetScale() const
	{
		logger.warn("Basis::GetScale() is not implemented!");
		return vec3();
	}

#define cofactor(row1, col1, row2, col2) \
	((*this)[row1][col1] * (*this)[row2][col2] - (*this)[row1][col2] * (*this)[row2][col1])

	Basis Basis::Inverse() const
	{
		float co[3] = {
			cofactor(1, 1, 2, 2), cofactor(1, 2, 2, 0), cofactor(1, 0, 2, 1)
		};

		float determinant =
			(*this)[0][0] * co[0] +
			(*this)[0][1] * co[1] +
			(*this)[0][2] * co[2];

		assert(determinant != 0, "Determinant == 0 in Basis::Inverse()!");

		float s = 1.f / determinant;

		mat3x3f result;

		result.data[0][0] = co[0] * s;
		result.data[0][1] = cofactor(0, 2, 2, 1) * s;
		result.data[0][2] = cofactor(0, 1, 1, 2) * s;

		result.data[1][0] = co[1] * s;
		result.data[1][1] = cofactor(0, 0, 2, 2) * s;
		result.data[1][2] = cofactor(0, 2, 1, 0) * s;

		result.data[2][0] = co[2] * s;
		result.data[2][1] = cofactor(0, 1, 2, 0) * s;
		result.data[2][2] = cofactor(0, 0, 1, 1) * s;

		return Basis(result);
	}

	Basis Basis::Orthonormalized() const
	{
		// Use the Gram-Schmidt process to orthonormalize

		vec3 x = normalize(xAxis);
		vec3 y = normalize(yAxis);
		vec3 z = normalize(zAxis);

		y = (y - x * dot(x, y));
		y = normalize(y);

		z = (z - x * dot(x, y) - y * dot(y, z));
		z = normalize(z);

		return Basis(z, y, z);
	}

	Basis Basis::Rotated(vec3 axis, float angle, bool local) const
	{
		mat3x3f mat = Matrix();

		if (!local)
			mat.rotate(axis, angle);
		else
			mat.rotate(axis * mat, angle);

		return Basis(mat);
	}

	Basis Basis::Scaled(vec3 scale, bool local) const
	{
		vec3 transformedScale = scale;

		if (local)
			transformedScale *= this->Matrix();

		return Basis(xAxis * scale.x, yAxis * scale.y, zAxis * scale.z);
	}

	Basis Basis::Lerp(Basis to, float weight) const
	{
		return Basis(lerp((*this)[0], to[0], weight), lerp((*this)[1], to[1], weight), lerp((*this)[2], to[2], weight));
	}

	Basis Basis::Slerp(Basis /*to*/, float /*weight*/) const
	{
		//Quaternion from = this->GetRotationQuaternion();
		//Quaternion toQuaternion = to.GetRotationQuaternion();

		//Basis result = Basis(from.slerp(toQuaternion, weight));
		//result[0] *= std::lerp(magnitude((*this)[0]), magnitude(to[0]), weight);
		//result[1] *= std::lerp(magnitude((*this)[1]), magnitude(to[1]), weight);
		//result[2] *= std::lerp(magnitude((*this)[2]), magnitude(to[2]), weight);
		
		//return result;
		return Basis();
	}

	Basis Basis::LookAt(const vec3& target, const vec3& up) const
	{
		assert(magnitude(target) < flt_cmp_epsilon, "Target vector can't be zero");
		assert(magnitude(up) < flt_cmp_epsilon, "Up vector can't be zero");

		vec3 z = normalize(target);
		vec3 x = cross(up, z);
		
		assert(magnitude(x) < flt_cmp_epsilon, "Target vector and up vector can't be parallel");

		x = normalize(x);
		vec3 y = cross(z, x);

		return Basis(x, y, z);
	}

	Basis Basis::Transposed() const
	{
		Basis result = *this;

		result[0][1] = result[1][0];
		result[0][2] = result[2][0];
		result[1][2] = result[2][1];

		return result;
	}

	mat3x3f Basis::Matrix() const
	{
		mat3x3f mat;

		mat.data[0][0] = xAxis[0];
		mat.data[0][1] = xAxis[1];
		mat.data[0][2] = xAxis[2];

		mat.data[1][0] = yAxis[0];
		mat.data[1][1] = yAxis[1];
		mat.data[1][2] = yAxis[2];

		mat.data[2][0] = zAxis[0];
		mat.data[2][1] = zAxis[1];
		mat.data[2][2] = zAxis[2];

		return mat;
	}

	Basis operator*(const Basis& lhs, const Basis& rhs)
	{
		mat3x3f mat = lhs.Matrix();
		mat3x3f mat2 = rhs.Matrix();

		return Basis(mat * mat2);
	}

	vec3 operator*(const Basis& lhs, const vec3& rhs)
	{
		return rhs * lhs.Matrix();
	}

	Basis operator*(const Basis& lhs, const float& rhs)
	{
		return Basis(lhs.xAxis * rhs, lhs.yAxis * rhs, lhs.zAxis * rhs);
	}

	Basis operator*(const Basis& lhs, const int& rhs)
	{
		const float rhsFloat = (const float)rhs;

		return Basis(lhs.xAxis * rhsFloat, lhs.yAxis * rhsFloat, lhs.zAxis * rhsFloat);
	}

	Basis operator/(const Basis& lhs, const float& rhs)
	{
		assert(rhs > 0, "Tried to divide by 0");

		return Basis(lhs.xAxis / rhs, lhs.yAxis / rhs, lhs.zAxis / rhs);
	}

	Basis operator/(const Basis& lhs, const int& rhs)
	{
		assert(rhs > 0, "Tried to divide by 0");

		const float rhsFloat = (const float)rhs;

		return Basis(lhs.xAxis / rhsFloat, lhs.yAxis / rhsFloat, lhs.zAxis / rhsFloat);
	}

	bool Basis::operator==(const Basis& rhs)
	{
		return this->xAxis == rhs.xAxis && this->yAxis == rhs.yAxis && this->zAxis == rhs.zAxis;
	}

	vec3& Basis::operator[](const size_t i)
	{
		assert(i < 3, "Index out of bounds");

		switch (i)
		{
		case 0:
			return xAxis;
		case 1:
			return yAxis;
		case 2:
			return zAxis;
		default:
			// This'll cause a memory leak, but I don't care because
			// it only happens when an error condition occurs

			logger.warn("Index out of bounds in Basis operator[]");
			return *new vec3();
		}
	}

	const vec3& Basis::operator[](const size_t i) const
	{
		assert(i < 3, "Index out of bounds");

		switch (i)
		{
		case 0:
			return xAxis;
		case 1:
			return yAxis;
		case 2:
			return zAxis;
		default:
			// This'll cause a memory leak, but I don't care because
			// it only happens when an error condition occurs

			logger.warn("Index out of bounds in Basis operator[]");
			return *new vec3();
		}
	}
}