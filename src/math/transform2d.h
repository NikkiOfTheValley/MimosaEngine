#pragma once
#include "mat4x4.h"
#include "basis.h"
#include <vector>

namespace math
{
	//! @brief Represents a transformation in 2D space
	class Transform2D
	{
	public:
		Transform2D() {};

		Transform2D(float rotation, vec2 position);

		Transform2D(float rotation, vec2 scale, float skew, vec2 position);

		Transform2D(vec2 xAxis, vec2 yAxis, vec2 origin);

		/*! @brief Returns the result of the linear interpolation between
		* this transform and `transform` by the given weight.
		*/
		Transform2D Interpolate(const Transform2D& transform, float weight) const;

		/*!
		 * @brief Checks if this and `transform` are approximately equal, depending on the `epsilon` global
		 * @param transform The transform to compare to
		 * @return The result of the comparison
		 */
		bool IsEqualApprox(const Transform2D& transform) const;

		/*!
		 * @brief Rotates this transform so X+ points at the target position
		 * @param target The target to look at
		 * @return The result of the operation
		 */
		Transform2D LookAt(vec2 target) const;

		/*!
		 * @brief Orthonormalizes the transform, resulting in a transform that only represents rotation
		 * @return The result of the operation
		 */
		Transform2D Orthonormalized() const;

		//! @brief Orthonormalizes the transform, resulting in a transform that only represents rotation
		void Orthonormalize();

		/*!
		 * @brief Rotates the transform by `angle`
		 * @param angle The angle to rotate
		 * @return The result of the rotation
		 */
		Transform2D Rotated(float angle, bool local = false) const;

		/*!
		 * @brief Rotates the transform by `angle`
		 * @param angle The angle to rotate
		 * @return The result of the rotation
		 */
		Transform2D Rotate(float angle, bool local = false);

		/*!
		 * @brief Scales the transform
		 * @param scale The scale to apply
		 * @param local Set to true if the operation is in local space
		 * @return The result of the operation
		 */
		Transform2D Scaled(vec2 scale, bool local = false) const;

		/*!
		 * @brief Scales the transform
		 * @param scale The scale to apply
		 * @param local Set to true if the operation is in local space
		 */
		void Scale(vec2 scale, bool local = false);

		/*!
		 * @brief Translates the transform
		 * @param offset The offset to apply
		 * @param local Set to true if the translation is in local space
		 * @return The result of the translation
		 */
		Transform2D Translated(vec2 offset, bool local = false) const;

		/*!
		 * @brief Translates the transform
		 * @param offset The offset to apply
		 * @param local Set to true if the translation is in local space
		 */
		void Translate(vec2 offset, bool local = false);

		/*!
		 * @brief Multiplies the two transforms together, resulting in a combination of the two
		 * @param rhs The right hand side of the operation
		 * @return The result of the operation
		 */
		Transform2D& operator*=(const Transform2D& rhs);

		/*!
		 * @brief Multiplies the positions by the transform, resulting in a transformed list of positions
		 * @param rhs The right hand side of the operation
		 * @param lhs The left hand side of the operation
		 * @return The result of the operation
		 */
		friend std::vector<vec2> operator*(const std::vector<vec2>& rhs, const Transform2D& lhs);

		/*!
		 * @brief Multiplies the two transforms together, resulting in a combination of the two
		 * @param rhs The right hand side of the operation
		 * @param lhs The left hand side of the operation
		 * @return The result of the operation
		 */
		friend Transform2D operator*(const Transform2D& lhs, const Transform2D& rhs);

	public:
		vec2 origin = vec2(0, 0);
		vec2 x = vec2(1, 0);
		vec2 y = vec2(0, 1);
	};

	/*!
	 * @brief Multiplies the positions by the transform, resulting in a transformed list of positions
	 * @param rhs The right hand side of the operation
	 * @param lhs The left hand side of the operation
	 * @return The result of the operation
	 */
	std::vector<vec2>& operator*=(const std::vector<vec2>& lhs, const Transform2D& rhs);
}
