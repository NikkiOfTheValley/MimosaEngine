#include <math/transform2d.h>

namespace math
{
	Transform2D::Transform2D(float rotation, vec2 position)
	{

	}

	Transform2D::Transform2D(float rotation, vec2 scale, float skew, vec2 position)
	{

	}

	Transform2D::Transform2D(vec2 xAxis, vec2 yAxis, vec2 origin)
	{

	}

	Transform2D Transform2D::Interpolate(const Transform2D& transform, float weight) const
	{
		vec2 newX = lerp(x, transform.x, weight);
		vec2 newY = lerp(y, transform.y, weight);
		vec2 newOrigin = lerp(origin, transform.origin, weight);

		return Transform2D(newX, newY, newOrigin);
	}

	bool Transform2D::IsEqualApprox(const Transform2D& transform) const
	{

	}

	Transform2D Transform2D::LookAt(vec2 target) const
	{

	}

	Transform2D Transform2D::Orthonormalized() const
	{

	}

	void Transform2D::Orthonormalize()
	{

	}

	Transform2D Transform2D::Rotated(float angle, bool local = false) const
	{

	}

	Transform2D Transform2D::Rotate(float angle, bool local = false)
	{

	}

	Transform2D Transform2D::Scaled(vec2 scale, bool local = false) const
	{

	}

	void Transform2D::Scale(vec2 scale, bool local = false)
	{

	}

	Transform2D Transform2D::Translated(vec2 offset, bool local = false) const
	{

	}

	void Transform2D::Translate(vec2 offset, bool local = false)
	{

	}
}