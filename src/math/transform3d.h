#pragma once
#include "mat4x4.h"
#include "basis.h"
#include <vector>

namespace math
{
	//! @brief Represents a transformation in 3D space
	class Transform3D
	{
	public:
		Transform3D() {};

		Transform3D(mat4x4f matrix);

		Transform3D(Basis basis, vec3 origin);

		/*!
		 * @brief Constructs a Transform3D from the basis axes and the origin
		 * @param xAxis The X axis of the basis
		 * @param yAxis The Y axis of the basis
		 * @param zAxis The Z axis of the basis
		 * @param origin The transform origin
		 */
		Transform3D(vec3 xAxis, vec3 yAxis, vec3 zAxis, vec3 origin);

		/*! @brief Returns the result of the linear interpolation between
		* this transform and `transform` by the given weight.
		*/
		Transform3D Interpolate(const Transform3D& transform, float weight) const;

		/*!
		 * @brief Checks if this and `transform` are approximately equal, depending on the `flt_cmp_epsilon` global
		 * @param transform The transform to compare to
		 * @return The result of the comparison
		 */
		bool IsEqualApprox(const Transform3D& transform) const;

		/*!
		 * @brief Rotates this transform so Z- points at the target position, with the Y+ axis being as close to `up` as possible
		 * @param target The target to look at
		 * @param up The up direction
		 * @return The result of the operation
		 */
		Transform3D LookAt(vec3 target, vec3 up = vec3(0, 1, 0)) const;

		/*!
		 * @brief Orthonormalizes the transform, resulting in a transform that only represents rotation
		 * @return The result of the operation
		 */
		Transform3D Orthonormalized() const;

		//! @brief Orthonormalizes the transform, resulting in a transform that only represents rotation
		void Orthonormalize();

		/*!
		 * @brief Rotates the transform around `axis` by `angle`
		 * @param axis The axis to rotate around
		 * @param angle The angle to rotate
		 * @param local Set to true if the rotation is in local space
		 * @return The result of the rotation
		 */
		Transform3D Rotated(vec3 axis, float angle, bool local = false) const;

		/*!
		 * @brief Rotates the transform around `axis` by `angle`
		 * @param axis The axis to rotate around
		 * @param angle The angle to rotate
		 * @param local Set to true if the rotation is in local space
		 */
		void Rotate(vec3 axis, float angle, bool local = false);

		/*!
		 * @brief Scales the transform
		 * @param scale The scale to apply
		 * @param local Set to true if the operation is in local space
		 * @return The result of the operation
		 */
		Transform3D Scaled(vec3 scale, bool local = false) const;

		/*!
		 * @brief Scales the transform
		 * @param scale The scale to apply
		 * @param local Set to true if the operation is in local space
		 */
		void Scale(vec3 scale, bool local = false);

		/*!
		 * @brief Translates the transform
		 * @param offset The offset to apply
		 * @param local Set to true if the translation is in local space
		 * @return The result of the translation
		 */
		Transform3D Translated(vec3 offset, bool local = false) const;

		/*!
		 * @brief Translates the transform
		 * @param offset The offset to apply
		 * @param local Set to true if the translation is in local space
		 */
		void Translate(vec3 offset, bool local = false);

		//! @brief Gets the origin of the transform (in other words, the translation part)
		const vec3 GetOrigin() const;

		//! @brief Sets the origin of the transform (in other words, the translation part)
		void SetOrigin(vec3 newOrigin);

		//! @brief Gets the basis of the transform
		Basis GetBasis() const;

		//! @brief Sets the basis of the transform
		void SetBasis(Basis newBasis);

		/*!
		 * @brief Multiplies the two transforms together, resulting in a combination of the two
		 * @param rhs The right hand side of the operation
		 */
		void operator*=(const Transform3D& rhs);

		/*!
		 * @brief Multiplies the positions by the transform, resulting in a transformed list of positions
		 * @param rhs The right hand side of the operation
		 * @param lhs The left hand side of the operation
		 * @return The result of the operation
		 */
		friend std::vector<vec3> operator*(const std::vector<vec3>& rhs, const Transform3D& lhs);

		/*!
		 * @brief Multiplies the two transforms together, resulting in a combination of the two
		 * @param rhs The right hand side of the operation
		 * @param lhs The left hand side of the operation
		 * @return The result of the operation
		 */
		friend Transform3D operator*(const Transform3D& lhs, const Transform3D& rhs);

	public:
		mat4x4f matrix;
	};

	/*!
	 * @brief Multiplies the positions by the transform, resulting in a transformed list of positions
	 * @param rhs The right hand side of the operation
	 * @param lhs The left hand side of the operation
	 */
	void operator*=(std::vector<vec3>& lhs, const Transform3D& rhs);
}
