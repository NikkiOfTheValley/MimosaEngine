#pragma once
#include "vec3.h"
#include "vec2.h"

vec3 sqrt(vec3 v);

vec3 invsqrt(vec3 v);

float invsqrt(float v);

// !! WARNING !! THIS IS PLATFORM-SPECIFIC AND ONLY WORKS ON x86!
float invsqrt_fast(float v);

vec3 cross(vec3 a, vec3 b);

float dot(vec3 a, vec3 b);

float dot(vec2 a, vec2 b);

vec3 normalize(vec3 v);

vec2 normalize(vec2 v);

// !! WARNING !! THIS IS PLATFORM-SPECIFIC AND ONLY WORKS ON x86!
vec3 normalize_fast(vec3 v);

// !! WARNING !! THIS IS PLATFORM-SPECIFIC AND ONLY WORKS ON x86!
vec2 normalize_fast(vec2 v);

bool is_nan(vec3 v);

bool is_nan(vec2 v);

float magnitude(vec3 v);

float magnitude(vec2 v);

float angleBetween(vec3 a, vec3 b);

// A and B must be normalized
float angleBetweenNormalized(vec3 a, vec3 b);

float squaredDist(vec3 a, vec3 b);

float squaredDist(vec2 a, vec2 b);

float dist(vec3 a, vec3 b);

float dist(vec2 a, vec2 b);

vec3 tripleProduct(vec3 a, vec3 b, vec3 c);

// Returns the barycentric coordinates of `point` for the triangle a, b, c
vec3 barycentricTriangle(vec3 a, vec3 b, vec3 c, vec3 point);

/*
Returns the point p projected onto the triangle a, b, c.
Assumes the point is within the triangle for performance reasons.
*/
vec3 projectPointOntoTri(vec3 a, vec3 b, vec3 c, vec3 p);