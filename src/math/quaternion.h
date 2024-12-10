#pragma once
#include "vec3.h"

namespace math
{
	//! @brief A implementation of quaternions for representing 3D rotations
	class Quaternion
	{
	public:
		float x = 0;
		float y = 0;
		float z = 0;
		float w = 1;

		//! @brief Constructs an identity quaternion, which represents no rotation
		Quaternion() {}

		//! @brief Constructs a quaternion that represents the arc between `arcFrom` and `arcTo`
		Quaternion(vec3 arcFrom, vec3 arcTo);

		//! @brief Constructs a quaternion from a axis-angle rotation
		Quaternion(vec3 axis, float angle);

		//! @brief Constructs a quaternion directly from its vector and real components
		Quaternion(float x, float y, float z, float w);

		//! @brief Computes the angle to `to` in radians
		float AngleTo(Quaternion to) const;

		/*!
		 * @brief Computes the dot product of two quaternions
		 * @param q The other quaternion to compute the dot product with 
		 * @return The computed dot product
		 */
		float dot(Quaternion q) const;

		//! @brief Returns the exponential of this quaternion
		Quaternion exp() const;

		//! @brief Returns the log of this quaternion
		Quaternion log() const;

		//! @brief Returns the inverse of this quaternion, meaning every component except W is flipped
		Quaternion inverse() const;

		//! @brief Returns the squared length of this quaternion
		float length_squared() const;

		//! @brief Returns the length of this quaternion
		float length() const;

		//! @brief Constructs a quaternion from Euler angles
		Quaternion FromEuler(vec3 euler);

		//! @brief Returns the axis of the rotation represented by this quaternion
		vec3 GetAxis() const;

		//! @brief Returns the angle of the rotation represented by this quaternion
		float GetAngle() const;

		//! @brief Returns the euler angles of this quaternion
		vec3 GetEuler() const;

		/*!
		 * @brief Returns true if two quaternions are approximately equal
		 * within the range of the `flt_cmp_epsilon` global
		 * @param q The quaternion to compare to
		 */
		bool IsEqualApprox(Quaternion q) const;

		//! @brief Checks if the quaternion is normalized
		bool IsNormalized() const;

		//! @brief Normalizes the quaternion
		Quaternion Normalized() const;

		/*!
		 * @brief Spherical linear interpolation
		 * @param to The quaternion to interpolate to
		 * @param weight The weight to use in the interpolation
		 * @return The result of the interpolation
		 */
		Quaternion slerp(Quaternion to, float weight) const;

		/*!
		 * @brief Spherical linear interpolation (no invert)
		 * @param to The quaternion to interpolate to
		 * @param weight The weight to use in the interpolation
		 * @return The result of the interpolation
		 */
		Quaternion slerpni(Quaternion to, float weight) const;

		/*!
		 * @brief Performs a spherical cubic interpolation between preA, this quaternion, b, and postB
		 * @param b
		 * @param preA 
		 * @param postB 
		 * @param weight The weight to use in the interpolation
		 * @return The resulting quaternion
		 */
		Quaternion SphericalCubicInterpolate(Quaternion b, Quaternion preA, Quaternion postB, float weight) const;

		/*!
		 * @brief Performs a spherical cubic interpolation between preA, this quaternion, b, and postB
		 * @param b
		 * @param preA
		 * @param postB
		 * @param bTime The time value at B
		 * @param preATime The time value at preA
		 * @param postBTime The time value at postB
		 * @param weight The weight to use in the interpolation
		 * @return The resulting quaternion
		 */
		Quaternion SphericalCubicInterpolateInTime(Quaternion b, Quaternion preA, Quaternion postB, float bTime, float preATime, float postBTime, float weight) const;
	

		
	};
}