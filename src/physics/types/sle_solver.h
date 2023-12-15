#pragma once
#include "../phys_globals.h"
#include "../core/globals.h"
#include "large_matrix.h"
#include "large_vector.h"

struct PhysState;

struct impulse
{
	LargeVector<MAX_PHYS_OBJECTS * 3> velocityImpulse;
	LargeVector<MAX_PHYS_OBJECTS * 3> angularVelocityImpulse;
};

// A very basic SLE solver used for solving equality constraints
class SLESolver
{
public:
	// Solves equality constraints on a given physics state
	impulse SolveConstraints(PhysState* initialState, LargeVector<MAX_PHYS_OBJECTS * 3> objPosVector, LargeVector<MAX_PHYS_OBJECTS * 3> objRotVector, double fixedDeltaTime);
};