#pragma once
#include <string>
#include <sstream>
#include "../core/log.h"
#include "../core/globals.h"

/*
Predefine class so no circular dependency is
required between vec3.h and mat4x4.h
*/

class mat4x4f;

struct vec2
{
public:
	// The components of the vector
	//float data[2];

	float x;
	float y;

	// Default constructor, initilizes the components to 0
	vec2();

	// Initilizes the components to x & y
	vec2(float x, float y);

	// Integer version of the float constructor
	vec2(int x, int y);


	vec2& operator+=(const vec2& rhs);

	vec2& operator+=(const float rhs);

	vec2& operator-=(const vec2& rhs);

	vec2& operator-=(const float rhs);

	vec2& operator*=(const vec2& rhs);

	vec2& operator*=(const float rhs);

	vec2& operator/=(const vec2& rhs);

	vec2& operator/=(const float rhs);


	friend vec2 operator+(const vec2& lhs, const vec2& rhs);

	friend vec2 operator+(const vec2& lhs, const float rhs);

	friend vec2 operator-(const vec2& lhs, const vec2& rhs);

	friend vec2 operator-(const vec2& lhs, const float rhs);

	friend vec2 operator*(const vec2& lhs, const vec2& rhs);

	friend vec2 operator*(const vec2& lhs, const float rhs);

	friend vec2 operator/(const vec2& lhs, const vec2& rhs);

	friend vec2 operator/(const vec2& lhs, const float rhs);


	bool operator==(const vec2& rhs);
	bool operator!=(const vec2& rhs);


	operator std::string() const;
};