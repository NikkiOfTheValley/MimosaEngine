#pragma once
#include "phys_state.h"

class PhysicsManager
{
public:
	void Init();

	void CreateObject(std::string name, vec3 pos, vec3 rot, vec3 vel = vec3(), vec3 angVel = vec3());

	PhysObj* GetObj(std::string name);

	void ApplyForce(std::string name, vec3 force);

	void ApplyAngularForce(std::string name, vec3 angForce);

private:
	PhysState state;
};