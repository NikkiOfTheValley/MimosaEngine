#pragma once
#include "../constraint.h"
#include "../collision_handler.h"

class CollisionConstraint : public Constraint
{
public:
	collision collision_data;

	virtual void Calculate(PhysState* state, LargeVector<MAX_PHYS_OBJECTS * 3> objPosVector, LargeVector<MAX_PHYS_OBJECTS * 3> objRotVector, double fixedDeltaTime);

	ConstraintType Type();
};