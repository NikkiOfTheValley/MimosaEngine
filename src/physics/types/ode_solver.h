#pragma once
#include "../phys_globals.h"
#include "../core/globals.h"
#include "large_vector.h"

struct PhysState;

struct obj_vel_and_accel
{
	// A copy of the objAccelVector structure from PhysState
	LargeVector<MAX_PHYS_OBJECTS * 6> objAccelVector;

	// A copy of the objVelVector structure from PhysState
	LargeVector<MAX_PHYS_OBJECTS * 6> objVelVector;
};

// An ODE solver implemented using the Runge-Kutta 4 method
class ODESolver
{
public:
	void Solve(PhysState* state, float fixedDeltaTime);

private:
	obj_vel_and_accel k1;
	obj_vel_and_accel k2;
	obj_vel_and_accel k3;
	obj_vel_and_accel k4;

	obj_vel_and_accel Evaluate(PhysState* state, float fixedDeltaTime, const obj_vel_and_accel& oldObjVelAndAccel);
};