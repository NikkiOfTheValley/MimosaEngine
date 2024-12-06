#include "../vec2.h"
#include "../mat4x4.h"
#include "../physics/types/assert.h"

namespace math
{
	// Implementation of vec2 default constructor from vec2.h
	vec2::vec2()
	{
		this->x = 0;
		this->y = 0;
	}

	// Implementation of vec2 float constructor from vec2.h
	vec2::vec2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	// Implementation of vec2 int constructor from vec2.h
	vec2::vec2(int x, int y)
	{
		//logger->warn("Initilization of vec2 with int constructor, this may result in data loss\n");

		this->x = (float)x;
		this->y = (float)y;
	}

	// Implementation of vec2 unsigned int constructor from vec2.h
	vec2::vec2(unsigned int x, unsigned int y)
	{
		//logger->warn("Initilization of vec2 with int constructor, this may result in data loss\n");

		this->x = (float)x;
		this->y = (float)y;
	}

	// Implementation of various operators

	vec2& vec2::operator+=(const vec2& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	vec2& vec2::operator+=(const float rhs)
	{
		this->x += rhs;
		this->y += rhs;
		return *this;
	}

	vec2& vec2::operator-=(const vec2& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}

	vec2& vec2::operator-=(const float rhs)
	{
		this->x -= rhs;
		this->y -= rhs;
		return *this;
	}

	vec2& vec2::operator*=(const vec2& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		return *this;
	}

	vec2& vec2::operator*=(const float rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		return *this;
	}

	vec2& vec2::operator/=(const vec2& rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		return *this;
	}

	vec2& vec2::operator/=(const float rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		return *this;
	}


	vec2 operator+(const vec2& lhs, const vec2& rhs)
	{
		return vec2{ lhs.x + rhs.x, lhs.y + rhs.y };
	}

	vec2 operator+(const vec2& lhs, const float rhs)
	{
		return vec2{ lhs.x + rhs, lhs.y + rhs };
	}

	vec2 operator-(const vec2& lhs, const vec2& rhs)
	{
		return vec2{ lhs.x - rhs.x, lhs.y - rhs.y };
	}

	vec2 operator-(const vec2& lhs, const float rhs)
	{
		return vec2{ lhs.x - rhs, lhs.y - rhs };
	}

	vec2 operator*(const vec2& lhs, const vec2& rhs)
	{
		return vec2{ lhs.x * rhs.x, lhs.y * rhs.y };
	}

	vec2 operator*(const vec2& lhs, const float rhs)
	{
		return vec2{ lhs.x * rhs, lhs.y * rhs };
	}

	vec2 operator/(const vec2& lhs, const vec2& rhs)
	{
		return vec2{ lhs.x / rhs.x, lhs.y / rhs.y };
	}

	vec2 operator/(const vec2& lhs, const float rhs)
	{
		return vec2{ lhs.x / rhs, lhs.y / rhs };
	}


	vec2 operator-(const vec2& lhs)
	{
		return vec2{ -lhs.x, -lhs.y };
	}


	bool vec2::operator==(const vec2& rhs)
	{
		return (this->x == rhs.x) && (this->y == rhs.y);
	}

	bool vec2::operator!=(const vec2& rhs)
	{
		return (this->x != rhs.x) && (this->y != rhs.y);
	}

	float& vec2::operator[](const size_t i)
	{
		assert(i < 2, "Index out of bounds when executing vec2::operator[]");
		return i == 0 ? x : y;
	}

	const float& vec2::operator[](const size_t i) const
	{
		assert(i < 2, "Index out of bounds when executing vec2::operator[]");
		return i == 0 ? x : y;
	}


	vec2::operator std::string() const {
		std::stringstream resultString;
		resultString << "{" << this->x << ", " << this->y << "}";

		return resultString.str();
	}
}