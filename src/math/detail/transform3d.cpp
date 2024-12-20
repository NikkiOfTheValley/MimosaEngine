#include <math/transform3d.h>
#include <physics/types/assert.h>

namespace math
{
	Transform3D::Transform3D(mat4x4f matrix)
	{
		this->matrix = matrix;
	}

	Transform3D::Transform3D(Basis basis, vec3 origin)
	{
		matrix.identity();

		SetBasis(basis);
		SetOrigin(origin);
	}

	Transform3D::Transform3D(vec3 xAxis, vec3 yAxis, vec3 zAxis, vec3 origin)
	{
		matrix.identity();

		SetBasis(Basis(xAxis, yAxis, zAxis));
		SetOrigin(origin);
	}

	Transform3D Transform3D::Interpolate(const Transform3D& transform, float weight) const
	{
		mat4x4f result;

		for (size_t i = 0; i < 4; i++)
			for (size_t k = 0; k < 4; k++)
				result.data[i][k] = std::lerp(this->matrix.data[i][k], transform.matrix.data[i][k], weight);

		return Transform3D(result);
	}

	bool Transform3D::IsEqualApprox(const Transform3D& transform) const
	{
		bool isNotEqual = false;

		for (size_t i = 0; i < 4; i++)
			for (size_t k = 0; k < 4; k++)
				isNotEqual |= abs(this->matrix.data[i][k] - transform.matrix.data[i][k]) > flt_cmp_epsilon;

		return !isNotEqual;
	}

	Transform3D Transform3D::LookAt(vec3 target, vec3 up) const
	{
		assert(dist(GetOrigin(), target) < flt_cmp_epsilon, "Transform origin and target are equal");

		Transform3D result = *this;

		result.SetBasis(result.GetBasis().LookAt(target - GetOrigin(), up));

		return result;
	}

	Transform3D Transform3D::Orthonormalized() const
	{
		return Transform3D(GetBasis().Orthonormalized(), GetOrigin());
	}

	void Transform3D::Orthonormalize()
	{
		SetBasis(GetBasis().Orthonormalized());
	}

	Transform3D Transform3D::Rotated(vec3 axis, float angle, bool local) const
	{
		mat4x4f rotationMatrix = mat4x4f();

		if (!local)
			rotationMatrix.rotate(axis, angle);
		else
			rotationMatrix.rotate(normalize(axis * matrix), angle);

		return matrix * rotationMatrix;
	}

	void Transform3D::Rotate(vec3 axis, float angle, bool local)
	{
		mat4x4f rotationMatrix = mat4x4f();

		if (!local)
			rotationMatrix.rotate(axis, angle);
		else
			rotationMatrix.rotate(normalize(axis * matrix), angle);

		this->matrix *= rotationMatrix;
	}

	Transform3D Transform3D::Scaled(vec3 scale, bool local) const
	{
		return Transform3D(GetBasis().Scaled(scale, local), GetOrigin());
	}

	void Transform3D::Scale(vec3 scale, bool local)
	{
		SetBasis(GetBasis().Scaled(scale, local));
	}

	Transform3D Transform3D::Translated(vec3 offset, bool local) const
	{
		vec3 newOrigin;

		if (!local)
			newOrigin = GetOrigin() + offset;
		else
			newOrigin = GetOrigin() + (offset * this->Orthonormalized());

		Transform3D transform = Transform3D(matrix);
		transform.SetOrigin(newOrigin);

		return transform;
	}

	void Transform3D::Translate(vec3 offset, bool local)
	{
		vec3 newOrigin;

		if (!local)
			newOrigin = GetOrigin() + offset;
		else
			newOrigin = GetOrigin() + (offset * this->Orthonormalized());

		SetOrigin(newOrigin);
	}

	const vec3 Transform3D::GetOrigin() const
	{
		return vec3(matrix.data[3][0], matrix.data[3][1], matrix.data[3][2]);
	}

	void Transform3D::SetOrigin(vec3 newOrigin)
	{
		matrix.data[3][0] = newOrigin.x;
		matrix.data[3][1] = newOrigin.y;
		matrix.data[3][2] = newOrigin.z;
	}

	Basis Transform3D::GetBasis() const
	{
		return Basis(matrix);
	}

	void Transform3D::SetBasis(Basis newBasis)
	{
		matrix.data[0][0] = newBasis.xAxis[0];
		matrix.data[0][1] = newBasis.xAxis[1];
		matrix.data[0][2] = newBasis.xAxis[2];

		matrix.data[1][0] = newBasis.yAxis[0];
		matrix.data[1][1] = newBasis.yAxis[1];
		matrix.data[1][2] = newBasis.yAxis[2];

		matrix.data[2][0] = newBasis.zAxis[0];
		matrix.data[2][1] = newBasis.zAxis[1];
		matrix.data[2][2] = newBasis.zAxis[2];
	}

	void Transform3D::operator*=(const Transform3D& rhs)
	{
		matrix *= rhs.matrix;
	}

	std::vector<vec3> operator*(const std::vector<vec3>& lhs, const Transform3D& rhs)
	{
		std::vector<vec3> result;

		for (auto& vec : lhs)
			result.push_back(vec * rhs);

		return result;
	}

	Transform3D operator*(const Transform3D& lhs, const Transform3D& rhs)
	{
		return Transform3D(lhs.matrix * rhs.matrix);
	}

	void operator*=(std::vector<vec3>& lhs, const Transform3D& rhs)
	{
		for (auto& vec : lhs)
			vec *= rhs;
	}
}