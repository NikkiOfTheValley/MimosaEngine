#pragma once
#include <vector>
#include "rendering/types/vec3.h"
#include "constraint.h"
#include "collision_mesh.h"
#include "constraints/collision_constraint.h"
#include <functional>

struct PhysState;
class CollisionMesh;

struct phys_obj_prop
{
	vec3 pos;
	vec3 rot;

	vec3 accel;
	vec3 angAccel;

	vec3 vel;
	vec3 angVel;

	float mass;
};

class PhysObj
{
public:
	PhysObj() {
		this->index = (size_t)-1;
	};

	PhysObj(PhysState* state, size_t index, bool hasGravity, std::function<void(double /* fixedDeltaTime */)> updateFunc = nullptr) : state(state), index(index), hasGravity(hasGravity), updateFunc(updateFunc) {};

	size_t index;
	bool hasGravity;
	bool isPinned;

	CollisionMesh collisionMesh;

	void Update(double fixedDeltaTime);

	phys_obj_prop GetProperties();

	void SetProperties(phys_obj_prop prop);
private:
	// This is private so it can only be modified on PhysObj creation,
	// otherwise some weird errors could occur if it's set in the middle of a physics step
	std::function<void(double /* fixedDeltaTime */)> updateFunc;

	PhysState* state;
};