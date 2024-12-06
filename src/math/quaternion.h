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

		Quaternion() {}

		Quaternion(vec3 arcFrom, vec3 arcTo);

		Quaternion(vec3 axis, float angle);

		Quaternion(float x, float y, float z, float w);

		float AngleTo(Quaternion to) const;

		float dot(Quaternion with) const;

		Quaternion exp() const;

		Quaternion log() const;

		Quaternion inverse() const;

		Quaternion FromEuler(vec3 euler);

		vec3 GetAxis() const;

		float GetAngle() const;

		vec3 GetEuler() const;

		bool IsEqualApprox(float epsilon) const;

		Quaternion Normalized() const;

		/*!
		 * @brief Spherical linear interpolation
		 * @param to 
		 * @param weight 
		 * @return 
		 */
		Quaternion slerp(Quaternion to, float weight) const;

		/*!
		 * @brief Spherical linear interpolation (no invert)
		 * @param to 
		 * @param weight 
		 * @return 
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