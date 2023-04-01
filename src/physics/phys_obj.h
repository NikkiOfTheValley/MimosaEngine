#pragma once
#include <vector>
#include "..\rendering\types\vec3.h"

struct PhysObj
{
	std::vector<Constraint*> constraints;

	vec3 pos;
	vec3 vel;
	vec3 accel;

	vec3 rot;
	vec3 angVel;
	vec3 angAccel;

	std::string name;
};