#include "../ode_solver.h"
#include "../phys_state.h"
#include "../sle_solver.h"

// This code is based on the RK4 implementation in https://gafferongames.com/post/integration_basics/

void ODESolver::Solve(
	PhysState* state,
	float fixedDeltaTime,
	std::function<force_and_torque(PhysState* state, obj_state& initialState, double fixedDeltaTime)> forceFunction,
	SLESolver* solver)
{
#if defined(ODE_SOLVER_TYPE_RK4)

	if (!haveDisplayedWarning)
	{
		Logger::getInstance().warn("ODE solver is using RK4 implementation! This is known to be broken!");
		haveDisplayedWarning = true;
	}
		

	Evaluate(state, 0.0f, {}, k1, forceFunction, solver);
	Evaluate(state, fixedDeltaTime * 0.5f, k1, k2, forceFunction, solver);
	Evaluate(state, fixedDeltaTime * 0.5f, k2, k3, forceFunction, solver);
	Evaluate(state, fixedDeltaTime, k3, k4, forceFunction, solver);

	state->objAccelVector = ((k1.objAccelVector + ((k2.objAccelVector + k3.objAccelVector) * 2.f) + k4.objAccelVector) * (1.f / 6.f)) * fixedDeltaTime;
	state->objAngAccelVector = ((k1.objAngAccelVector + ((k2.objAngAccelVector + k3.objAngAccelVector) * 2.f) + k4.objAngAccelVector) * (1.f / 6.f)) * fixedDeltaTime;

	state->objPositionVector += ((k1.objVelVector + ((k2.objVelVector + k3.objVelVector) * 2.f) + k4.objVelVector) * (1.f / 6.f)) * fixedDeltaTime;
	state->objRotationVector += ((k1.objAngVelVector + ((k2.objAngVelVector + k3.objAngVelVector) * 2.f) + k4.objAngVelVector) * (1.f / 6.f)) * fixedDeltaTime;

	state->objVelVector += state->objAccelVector;
	state->objAngVelVector += state->objAngAccelVector;

#elif defined(ODE_SOLVER_TYPE_EULER)

	impulse constraintImpulse = solver->SolveConstraints(state, state->objPositionVector, state->objRotationVector, fixedDeltaTime);

	state->objAccelVector = state->objForceVector / state->objMass;

	//for (auto& obj : state->objects)
	//{
		//if (obj.index >= MAX_PHYS_OBJECTS)
		//	continue;

		//vec3* angAccel = state->objAngAccelVector.GetVector<vec3>(obj.index * 3);

		//const mat3x3f& inertiaTensor = state->objInertiaMatrices[obj.index];

		//*angAccel * inertiaTensor;
	//}


	state->objAngAccelVector = state->objTorqueVector / state->objMass;

	state->objVelVector += state->objAccelVector + constraintImpulse.velocityImpulse;
	state->objAngVelVector += state->objAngAccelVector + constraintImpulse.angularVelocityImpulse;

	state->objPositionVector += state->objVelVector * fixedDeltaTime;
	state->objRotationVector += state->objAngVelVector * fixedDeltaTime;
#endif
}

void ODESolver::Evaluate(
	PhysState* initialState,
	float fixedDeltaTime,
	const obj_state& oldState,
	obj_state& output,
	std::function<force_and_torque (PhysState* initialState, obj_state& state, double fixedDeltaTime)> forceFunction,
	SLESolver* solver)
{
	tempObjPosVector = initialState->objPositionVector + oldState.objVelVector * fixedDeltaTime;
	tempObjRotVector = initialState->objRotationVector + oldState.objAngVelVector * fixedDeltaTime;

	impulse constraintImpulse = solver->SolveConstraints(initialState, tempObjPosVector, tempObjRotVector, fixedDeltaTime);

	output.objVelVector = (initialState->objVelVector + oldState.objAccelVector + constraintImpulse.velocityImpulse);
	output.objAngVelVector = (initialState->objAngVelVector + oldState.objAngAccelVector + constraintImpulse.angularVelocityImpulse);

	force_and_torque forceAndTorque = forceFunction(initialState, output, fixedDeltaTime);

	output.objAccelVector = (initialState->objForceVector + forceAndTorque.forceVector) / initialState->objMass;
	output.objAngAccelVector = (initialState->objTorqueVector + forceAndTorque.torqueVector) / initialState->objMass;
}