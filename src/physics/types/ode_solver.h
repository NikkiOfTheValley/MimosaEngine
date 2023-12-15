#pragma once
#include "../phys_globals.h"
#include "../core/globals.h"
#include "large_vector.h"
#include <functional>

// This code is based on the RK4 implementation in https://gafferongames.com/post/integration_basics/

struct PhysState;
class SLESolver;

struct obj_state
{
	// A copy of the objAccelVector structure from PhysState
	LargeVector<MAX_PHYS_OBJECTS * 3> objAccelVector;

	// A copy of the objAngAccel structure from PhysState
	LargeVector<MAX_PHYS_OBJECTS * 3> objAngAccelVector;

	// A copy of the objVelVector structure from PhysState
	LargeVector<MAX_PHYS_OBJECTS * 3> objVelVector;

	// A copy of the objAngVel structure from PhysState
	LargeVector<MAX_PHYS_OBJECTS * 3> objAngVelVector;
};

struct force_and_torque
{
	LargeVector<MAX_PHYS_OBJECTS * 3> forceVector;
	LargeVector<MAX_PHYS_OBJECTS * 3> torqueVector;
};

// An ODE solver implemented using the Runge-Kutta 4 method
class ODESolver
{
public:
	void Solve(
		PhysState* state,
		float fixedDeltaTime,
		std::function<force_and_torque(PhysState* initialState, obj_state& state, double fixedDeltaTime)> forceFunction,
		SLESolver* solver);

private:
	obj_state k1;
	obj_state k2;
	obj_state k3;
	obj_state k4;

	LargeVector<MAX_PHYS_OBJECTS * 3> tempObjPosVector;
	LargeVector<MAX_PHYS_OBJECTS * 3> tempObjRotVector;

	void Evaluate(PhysState* state,
		float fixedDeltaTime,
		const obj_state& oldState,
		obj_state& output,
		std::function<force_and_torque(PhysState* initialState, obj_state& state, double fixedDeltaTime)> forceFunction,
		SLESolver* solver);
};