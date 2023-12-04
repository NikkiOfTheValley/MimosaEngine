#include "../ode_solver.h"
#include "../phys_state.h"

void ODESolver::Solve(PhysState* state, float fixedDeltaTime)
{
	k1 = Evaluate(state, 0.0f, {});
	k2 = Evaluate(state, fixedDeltaTime * 0.5f, k1);
	k3 = Evaluate(state, fixedDeltaTime * 0.5f, k2);
	k4 = Evaluate(state, fixedDeltaTime, k3);

	LargeVector<MAX_PHYS_OBJECTS * 6> tempObjVelVector = (k1.objVelVector + ((k2.objVelVector + k3.objVelVector) * 2.f) + k4.objVelVector) * (1.f / 6.f);
	state->objAccelVector = ((k1.objAccelVector + ((k2.objAccelVector + k3.objAccelVector) * 2.f) + k4.objAccelVector) * (1.f / 6.f)) * fixedDeltaTime;

	state->objStateVector += tempObjVelVector * fixedDeltaTime;
	state->objVelVector += state->objAccelVector;
}

obj_vel_and_accel ODESolver::Evaluate(PhysState* state, float fixedDeltaTime, const obj_vel_and_accel& oldObjVelAndAccel)
{
	// Create a copy of the objStateVector

	LargeVector<MAX_PHYS_OBJECTS * 6> tempObjStateVector = state->objStateVector;

	tempObjStateVector += oldObjVelAndAccel.objVelVector * fixedDeltaTime;

	obj_vel_and_accel output;
	output.objVelVector = state->objVelVector + oldObjVelAndAccel.objAccelVector;
	output.objAccelVector = (state->objForceVector * state->objPropertiesMatrix.inverseDiagonal()); 

	return output;
}