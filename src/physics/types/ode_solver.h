#pragma once
#include "../phys_globals.h"
#include "../core/globals.h"
#include "large_vector.h"

struct PhysState;

// An ODE solver implemented using the Runge-Kutta 4 method
class ODESolver
{
public:
	void Solve(PhysState* state, float fixedDeltaTime);

private:
	LargeVector<MAX_PHYS_OBJECTS * 6> k2;
	LargeVector<MAX_PHYS_OBJECTS * 6> k3;
	LargeVector<MAX_PHYS_OBJECTS * 6> k4;
};