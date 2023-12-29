#include "collision_constraint.h"

void CollisionConstraint::Calculate(PhysState* /*state*/, LargeVector<MAX_PHYS_OBJECTS * 3> /*objPosVector*/, LargeVector<MAX_PHYS_OBJECTS * 3> /*objRotVector*/, double /*fixedDeltaTime*/)
{

}

ConstraintType CollisionConstraint::Type()
{
	return ConstraintType::COLLISION;
}