#pragma once
#include "../phys_globals.h"
#include "core/globals.h"
#include "large_vector.h"
#include <functional>

// This code is based on the RK4 implementation in https://gafferongames.com/post/integration_basics/

struct PhysState;
class SLESolver;

/*!
 * @brief A simplified version of the physics engine state
 * @var objAccelVector A copy of the objAccelVector structure from `PhysState`
 * @var objAngAccelVector A copy of the objAngAccel structure from `PhysState`
 * @var objVelVector A copy of the objVelVector structure from `PhysState`
 * @var objAngVelVector A copy of the objAngVel structure from `PhysState`
*/
struct obj_state
{
	LargeVector<MAX_PHYS_OBJECTS * 3> objAccelVector;
	LargeVector<MAX_PHYS_OBJECTS * 3> objAngAccelVector;
	LargeVector<MAX_PHYS_OBJECTS * 3> objVelVector;
	LargeVector<MAX_PHYS_OBJECTS * 3> objAngVelVector;
};

struct force_and_torque
{
	LargeVector<MAX_PHYS_OBJECTS * 3> forceVector;
	LargeVector<MAX_PHYS_OBJECTS * 3> torqueVector;
};

//! @brief An ODE solver that can be switched between RK4 and Euler
class ODESolver
{
public:
	/*!
	 * @brief Solves for the physics state
	 * @param[in,out] state The physics state to solve from
	 * @param fixedDeltaTime The deltaTime to use
	 * @param forceFunction The force function, is used to calcuate forces from a given state. Only used in RK4 mode
	 * @param[in] solver The SLE solver to use
	*/
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

	/*!
	 * @brief Evaluates an RK4 stage
	 * @param[in] initialState The initial state
	 * @param fixedDeltaTime The deltaTime to use
	 * @param[in] oldState The previous state
	 * @param[out] output The output state
	 * @param forceFunction The force function
	 * @param solver The SLE solver to use
	*/
	void Evaluate(PhysState* initialState,
		float fixedDeltaTime,
		const obj_state& oldState,
		obj_state& output,
		std::function<force_and_torque(PhysState* initialState, obj_state& state, double fixedDeltaTime)> forceFunction,
		SLESolver* solver);

	bool haveDisplayedWarning = false;
};