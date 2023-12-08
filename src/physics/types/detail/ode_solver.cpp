#include "../ode_solver.h"
#include "../phys_state.h"

void ODESolver::Solve(PhysState* state, float fixedDeltaTime)
{
	Evaluate(state, 0.0f, {}, k1);
	Evaluate(state, fixedDeltaTime * 0.5f, k1, k2);
	Evaluate(state, fixedDeltaTime * 0.5f, k2, k3);
	Evaluate(state, fixedDeltaTime, k3, k4);

	state->objAccelVector = ((k1.objAccelVector + ((k2.objAccelVector + k3.objAccelVector) * 2.f) + k4.objAccelVector) * (1.f / 6.f)) * fixedDeltaTime;

	state->objStateVector += (k1.objVelVector + ((k2.objVelVector + k3.objVelVector) * 2.f) + k4.objVelVector) * (1.f / 6.f) * fixedDeltaTime;
	state->objVelVector += state->objAccelVector;
}

void ODESolver::Evaluate(PhysState* state, float fixedDeltaTime, const obj_vel_and_accel& oldObjVelAndAccel, obj_vel_and_accel& output)
{
	// Create a copy of the objStateVector
	tempObjStateVector = state->objStateVector;

	tempObjStateVector += oldObjVelAndAccel.objVelVector * fixedDeltaTime;

	output.objVelVector = state->objVelVector + oldObjVelAndAccel.objAccelVector;
	output.objAccelVector = state->objForceVector * state->objPropertiesMatrixInverse;
}