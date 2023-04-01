#include "../vec_math.h"
#include <intrin.h>

vec3 sqrt(vec3 v)
{
	return vec3(sqrt(v.data[0]), sqrt(v.data[1]), sqrt(v.data[2]));
}

vec3 invsqrt(vec3 v)
{
	return vec3(1 / sqrt(v.data[0]), 1 / sqrt(v.data[1]), 1 / sqrt(v.data[2]));
}

float invsqrt(float v)
{
	return 1 / sqrt(v);
}

inline void invsqrt_fast_internal(float* v, float* result)
{
	__m128 in = _mm_load_ss(v);
	_mm_store_ss(result, _mm_rsqrt_ps(in));
}

// !! WARNING !! THIS IS PLATFORM-SPECIFIC AND ONLY WORKS ON x86!
inline float invsqrt_fast(float v)
{
	float out;
	invsqrt_fast_internal(&v, &out);
	return out;
}

vec3 cross(vec3 a, vec3 b)
{
	return vec3(
		a.data[1] * b.data[2] - b.data[1] * a.data[2],
		a.data[2] * b.data[0] - b.data[2] * a.data[0],
		a.data[0] * b.data[1] - b.data[0] * a.data[1]);
}

float dot(vec3 a, vec3 b)
{
	vec3 tmp(a * b);
	return tmp.data[0] + tmp.data[1] + tmp.data[2];
}

float dot(vec2 a, vec2 b)
{
	vec2 tmp(a * b);
	return tmp.data[0] + tmp.data[1];
}

vec3 normalize(vec3 v)
{
	return v * (invsqrt(dot(v, v)));
}

// !! WARNING !! THIS IS PLATFORM-SPECIFIC AND ONLY WORKS ON x86!
vec3 normalize_fast(vec3 v)
{
	return v * (invsqrt_fast(dot(v, v)));
}

vec2 normalize(vec2 v)
{
	return v * (invsqrt(dot(v, v)));
}

// !! WARNING !! THIS IS PLATFORM-SPECIFIC AND ONLY WORKS ON x86!
vec2 normalize_fast(vec2 v)
{
	return v * (invsqrt_fast(dot(v, v)));
}

bool is_nan(vec3 v)
{
	return isnan(v.data[0]) || isnan(v.data[1]) || isnan(v.data[2]);
}

bool is_nan(vec2 v)
{
	return isnan(v.data[0]) || isnan(v.data[1]);
}

float magnitude(vec3 v)
{
	return sqrt((v.data[0] * v.data[0]) + (v.data[1] * v.data[1]) + (v.data[2] * v.data[2]));
}

float magnitude(vec2 v)
{
	return sqrt((v.data[0] * v.data[0]) + (v.data[1] * v.data[1]));
}

float angleBetween(vec3 a, vec3 b)
{
	return acos(dot(a, b) / (magnitude(a) * magnitude(b)));
}

// A and B must be normalized
float angleBetweenNormalized(vec3 a, vec3 b)
{
	return acos(dot(a, b));
}