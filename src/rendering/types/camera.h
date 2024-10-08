#pragma once
#include <math/vec.h>
#include <math/mat4x4.h>
#include "../renderer.h"

class Camera
{
public:
	float fov;
	math::vec3 pos;

	void Update();

	math::mat4x4f GetViewMatrix();
	math::mat4x4f GetProjectionMatrix();
	math::vec3 GetRotation();
private:
	math::vec3 rot;

	math::mat4x4f viewMatrix;
	math::mat4x4f projMatrix;

	math::vec2 prevMousePos;
};