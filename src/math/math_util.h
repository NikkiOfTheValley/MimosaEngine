#pragma once

namespace math
{
	//! @brief Computes the sign value of the given float
	constexpr float SIGN(const float v)
	{
		return v > 0 ? +1.0f : (v < 0 ? -1.0f : 0.0f);
	}
}