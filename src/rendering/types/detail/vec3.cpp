#include "../vec3.h"
#include "../mat4x4.h"
#include "../mat3x3.h"
#include "../physics/types/assert.h"

vec3::vec3()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

vec3::vec3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

vec3::vec3(int x, int y, int z)
{
	this->x = (float)x;
	this->y = (float)y;
	this->z = (float)z;
}

vec3::vec3(unsigned int x, unsigned int y, unsigned int z)
{
	this->x = (float)x;
	this->y = (float)y;
	this->z = (float)z;

}


vec3& vec3::operator+=(const vec3& rhs)
{
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

vec3& vec3::operator+=(const float rhs)
{
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}

vec3& vec3::operator-=(const vec3& rhs)
{
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

vec3& vec3::operator-=(const float rhs)
{
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}

vec3& vec3::operator*=(const vec3& rhs)
{
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}

vec3& vec3::operator*=(const float rhs)
{
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}

vec3& vec3::operator*=(const mat4x4f rhs)
{
	this->x = this->x * rhs.data[0][0] + this->y * rhs.data[0][1] + this->z * rhs.data[0][2] + 1 * rhs.data[0][3];
	this->y = this->x * rhs.data[1][0] + this->y * rhs.data[1][1] + this->z * rhs.data[1][2] + 1 * rhs.data[1][3];
	this->z = this->x * rhs.data[2][0] + this->y * rhs.data[2][1] + this->z * rhs.data[2][2] + 1 * rhs.data[2][3];
	
	return *this;
}

vec3& vec3::operator*=(const mat3x3f rhs)
{
	this->x = this->x * rhs.data[0][0] + this->y * rhs.data[0][1] + this->z * rhs.data[0][2];
	this->y = this->x * rhs.data[1][0] + this->y * rhs.data[1][1] + this->z * rhs.data[1][2];
	this->z = this->x * rhs.data[2][0] + this->y * rhs.data[2][1] + this->z * rhs.data[2][2];

	return *this;
}

vec3& vec3::operator/=(const vec3& rhs)
{
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}

vec3& vec3::operator/=(const float rhs)
{
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}


vec3 operator+(const vec3& lhs, const vec3& rhs)
{
	return vec3{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

vec3 operator+(const vec3& lhs, const float rhs)
{
	return vec3{ lhs.x + rhs, lhs.y + rhs, lhs.z + rhs };
}

vec3 operator-(const vec3& lhs, const vec3& rhs)
{
	return vec3{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

vec3 operator-(const vec3& lhs, const float rhs)
{
	return vec3{ lhs.x - rhs, lhs.y - rhs, lhs.z - rhs };
}

vec3 operator*(const vec3& lhs, const vec3& rhs)
{
	return vec3{ lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}

vec3 operator*(const vec3& lhs, const float rhs)
{
	return vec3{ lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}

vec3 operator*(const vec3& lhs, const mat4x4f rhs)
{
	return vec3{
		lhs.x * rhs.data[0][0] + lhs.y * rhs.data[0][1] + lhs.z * rhs.data[0][2] + 1 * rhs.data[0][3],
		lhs.x * rhs.data[1][0] + lhs.y * rhs.data[1][1] + lhs.z * rhs.data[1][2] + 1 * rhs.data[1][3],
		lhs.x * rhs.data[2][0] + lhs.y * rhs.data[2][1] + lhs.z * rhs.data[2][2] + 1 * rhs.data[2][3]};
}

vec3 operator*(const vec3& lhs, const mat3x3f rhs)
{
	return vec3{
		lhs.x * rhs.data[0][0] + lhs.y * rhs.data[0][1] + lhs.z * rhs.data[0][2],
		lhs.x * rhs.data[1][0] + lhs.y * rhs.data[1][1] + lhs.z * rhs.data[1][2],
		lhs.x * rhs.data[2][0] + lhs.y * rhs.data[2][1] + lhs.z * rhs.data[2][2] };
}

vec3 operator/(const vec3& lhs, const vec3& rhs)
{
	return vec3{ lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}

vec3 operator/(const vec3& lhs, const float rhs)
{
	return vec3{ lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
}


vec3 operator-(const vec3& lhs)
{
	return vec3{ -lhs.x, -lhs.y, -lhs.z };
}


bool vec3::operator==(const vec3& rhs)
{
	return (this->x == rhs.x) && (this->y == rhs.y) && (this->z == rhs.z);
}

bool vec3::operator!=(const vec3& rhs)
{
	return (this->x != rhs.x) && (this->y != rhs.y) && (this->z != rhs.z);
}

float& vec3::operator[](const size_t i)
{
	assert(i < 3, "Index out of bounds when executing vec3::operator[]");
	
	switch (i)
	{
	case 0:
		return x;
		break;

	case 1:
		return y;
		break;

	case 2:
		return z;
		break;
	}

	// This is here for when the index is out of bounds while asserts are turned off
	return x;
}


vec3::operator std::string() const {
	std::stringstream resultString;
	resultString << "{" << this->x << ", " << this->y << ", " << this->z << "}";

	return resultString.str();
}