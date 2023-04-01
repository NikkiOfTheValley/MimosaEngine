#include "../vec2.h"
#include "../mat4x4.h"

// Implementation of vec2 default constructor from vec2.h
vec2::vec2()
{
	data[0] = 0;
	data[1] = 0;
}

// Implementation of vec2 float constructor from vec2.h
vec2::vec2(float x, float y)
{
	data[0] = x;
	data[1] = y;
}

// Implementation of vec2 int constructor from vec2.h
vec2::vec2(int x, int y)
{
	//logger->warn("Initilization of vec2 with int constructor, this may result in data loss\n");

	data[0] = (float)x;
	data[1] = (float)y;
}

// Implementation of various operators

vec2& vec2::operator+=(const vec2& rhs)
{
	this->data[0] += rhs.data[0];
	this->data[1] += rhs.data[1];
	return *this;
}

vec2& vec2::operator+=(const float rhs)
{
	this->data[0] += rhs;
	this->data[1] += rhs;
	return *this;
}

vec2& vec2::operator-=(const vec2& rhs)
{
	this->data[0] -= rhs.data[0];
	this->data[1] -= rhs.data[1];
	return *this;
}

vec2& vec2::operator-=(const float rhs)
{
	this->data[0] -= rhs;
	this->data[1] -= rhs;
	return *this;
}

vec2& vec2::operator*=(const vec2& rhs)
{
	this->data[0] *= rhs.data[0];
	this->data[1] *= rhs.data[1];
	return *this;
}

vec2& vec2::operator*=(const float rhs)
{
	this->data[0] *= rhs;
	this->data[1] *= rhs;
	return *this;
}

vec2& vec2::operator/=(const vec2& rhs)
{
	this->data[0] /= rhs.data[0];
	this->data[1] /= rhs.data[1];
	return *this;
}

vec2& vec2::operator/=(const float rhs)
{
	this->data[0] /= rhs;
	this->data[1] /= rhs;
	return *this;
}


vec2 operator+(const vec2& lhs, const vec2& rhs)
{
	return vec2{ lhs.data[0] + rhs.data[0], lhs.data[1] + rhs.data[1]};
}

vec2 operator+(const vec2& lhs, const float rhs)
{
	return vec2{ lhs.data[0] + rhs, lhs.data[1] + rhs };
}

vec2 operator-(const vec2& lhs, const vec2& rhs)
{
	return vec2{ lhs.data[0] - rhs.data[0], lhs.data[1] - rhs.data[1] };
}

vec2 operator-(const vec2& lhs, const float rhs)
{
	return vec2{ lhs.data[0] - rhs, lhs.data[1] - rhs };
}

vec2 operator*(const vec2& lhs, const vec2& rhs)
{
	return vec2{ lhs.data[0] * rhs.data[0], lhs.data[1] * rhs.data[1] };
}

vec2 operator*(const vec2& lhs, const float rhs)
{
	return vec2{ lhs.data[0] * rhs, lhs.data[1] * rhs };
}

vec2 operator/(const vec2& lhs, const vec2& rhs)
{
	return vec2{ lhs.data[0] / rhs.data[0], lhs.data[1] / rhs.data[1] };
}

vec2 operator/(const vec2& lhs, const float rhs)
{
	return vec2{ lhs.data[0] / rhs, lhs.data[1] / rhs };
}


bool vec2::operator==(const vec2& rhs)
{
	return (this->data[0] == rhs.data[0]) && (this->data[1] == rhs.data[1]);
}

bool vec2::operator!=(const vec2& rhs)
{
	return (this->data[0] != rhs.data[0]) && (this->data[1] != rhs.data[1]);
}


vec2::operator std::string() const {
	std::stringstream resultString;
	resultString << "{" << data[0] << ", " << data[1] << "}";

	return resultString.str();
}