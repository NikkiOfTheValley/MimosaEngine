#pragma once
#include "vec.h"
#include "mat4x4.h"
#include "../renderer.h"

class Camera
{
public:
	float fov;
	vec3 pos;

	void Update();

	mat4x4f GetViewMatrix();
	mat4x4f GetProjectionMatrix();
	vec3 GetRotation();
private:
	vec3 rot;

	mat4x4f viewMatrix;
	mat4x4f projMatrix;

	vec2 prevMousePos;
};