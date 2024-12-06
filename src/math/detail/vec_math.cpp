#include "../vec_math.h"
#include <intrin.h>

namespace math
{
	vec3 sqrt(vec3 v)
	{
		return vec3(std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z));
	}

	vec3 invsqrt(vec3 v)
	{
		return vec3(1 / std::sqrt(v.x), 1 / std::sqrt(v.y), 1 / std::sqrt(v.z));
	}

	float invsqrt(float v)
	{
		return 1 / std::sqrt(v);
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
			a.y * b.z - b.y * a.z,
			a.z * b.x - b.z * a.x,
			a.x * b.y - b.x * a.y);
	}

	float dot(vec3 a, vec3 b)
	{
		vec3 tmp(a * b);
		return tmp.x + tmp.y + tmp.z;
	}

	float dot(vec2 a, vec2 b)
	{
		vec2 tmp(a * b);
		return tmp.x + tmp.y;
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
		return isnan(v.x) || isnan(v.y) || isnan(v.z);
	}

	bool is_nan(vec2 v)
	{
		return isnan(v.x) || isnan(v.y);
	}

	float magnitude(vec3 v)
	{
		return std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	}

	float magnitude(vec2 v)
	{
		return std::sqrt((v.x * v.x) + (v.y * v.y));
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

	float squaredDist(vec3 a, vec3 b)
	{
		float squaredDistX = (b.x - a.x) * (b.x - a.x);
		float squaredDistY = (b.y - a.y) * (b.y - a.y);
		float squaredDistZ = (b.z - a.z) * (b.z - a.z);

		return squaredDistX + squaredDistY + squaredDistZ;
	}

	float squaredDist(vec2 a, vec2 b)
	{
		float squaredDistX = (b.x - a.x) * (b.x - a.x);
		float squaredDistY = (b.y - a.y) * (b.y - a.y);

		return squaredDistX + squaredDistY;
	}

	float dist(vec3 a, vec3 b)
	{
		return std::sqrt(squaredDist(a, b));
	}

	float dist(vec2 a, vec2 b)
	{
		return std::sqrt(squaredDist(a, b));
	}

	vec3 lerp(vec3 a, vec3 b, float weight)
	{
		return (a * (1.f - weight)) + (b * weight);
	}

	vec2 lerp(vec2 a, vec2 b, float weight)
	{
		return (a * (1.f - weight)) + (b * weight);
	}

	vec3 tripleProduct(vec3 a, vec3 b, vec3 c)
	{
		return cross(a, cross(b, c));
	}

	vec3 barycentricTriangle(vec3 a, vec3 b, vec3 c, vec3 point)
	{
		// This code is from Chister Ericson's book "Real-Time Collision Detection"

		vec3 v0 = b - a;
		vec3 v1 = c - a;
		vec3 v2 = point - a;

		float d00 = dot(v0, v0);
		float d01 = dot(v0, v1);
		float d11 = dot(v1, v1);
		float d20 = dot(v2, v0);
		float d21 = dot(v2, v1);

		float denom = d00 * d11 - d01 * d01;

		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;

		return vec3(v, w, 1.0f - v - w);
	}

	vec3 projectPointOntoTri(vec3 a, vec3 b, vec3 c, vec3 p)
	{
		// This code is from Chister Ericson's book "Real-Time Collision Detection"

		// Determine if point lies in vertex regions
		vec3 AB = b - a;
		vec3 AC = c - a;

		vec3 AP = p - a;
		float dA1 = dot(AB, AP);
		float dA2 = dot(AC, AP);
		if (dA1 <= 0 and dA2 <= 0) return a; // vertex region a (barycentric coords 1,0,0)

		vec3 BP = p - b;
		float dB1 = dot(AB, BP);
		float dB2 = dot(AC, BP);
		if (dB1 >= 0 && dB2 <= 0) return b; // vertex region b (barycentric coords 0,1,0)

		vec3 CP = p - c;
		float dC1 = dot(AB, CP);
		float dC2 = dot(AC, CP);
		if (dC2 >= 0 && dC1 <= dC2) return c; // vertex region c (barycentric coords 0,0,1)

		float EdgeAB = dA1 * dB2 - dB1 * dA2;
		float EdgeBC = dB1 * dC2 - dC1 * dB2;
		float EdgeAC = dC1 * dA2 - dA1 * dC2;


		float summed_area = EdgeAB + EdgeBC + EdgeAC;
		float v = EdgeAC / summed_area;
		float w = EdgeAB / summed_area;
		return a + AB * v + AC * w;
	}
}