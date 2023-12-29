#pragma once
#include "large_matrix.h"
#include "large_vector.h"
#include "../phys_globals.h"

class PhysObj;
struct PhysState;

class Constraint
{
public:
	virtual void Calculate(PhysState* state, LargeVector<MAX_PHYS_OBJECTS * 3> objPosVector, LargeVector<MAX_PHYS_OBJECTS * 3> objRotVector, double fixedDeltaTime);

	// Returns its constraint type
	virtual ConstraintType Type();

	// The constraint's Jacobian is 6x4
	// Each row of the Jacobian is for 1 constraint
	// (Each Constraint object can act on the 6 total elements of the position and rotation vectors, which can be decomposed into 6 individual constraints)

	LargeMatrix<6, 4> jacobian;
};