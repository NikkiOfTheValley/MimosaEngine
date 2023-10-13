#pragma once
#include "../constraint.h"
#include "../collision_mesh.h"

class CollisionConstraint : public Constraint
{
public:
	CollisionMesh collisionMesh;

	void Apply(PhysObj* obj);
};