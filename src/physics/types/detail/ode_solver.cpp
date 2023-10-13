#include "../ode_solver.h"
#include "../phys_state.h"

void ODESolver::Solve(PhysState* state, float fixedDeltaTime)
{
	// Acceleration = Force / Mass, so objAccelVector = objForceVector * objPropertiesMatrix.inverseDiagonal()

	state->objAccelVector = state->objForceVector * state->objPropertiesMatrix.inverseDiagonal();

	// Apply gravity in the Y direction
	for (auto& obj : state->objects)
		if (obj.hasGravity)
			state->objAccelVector[(obj.index * 6) + 1] = -9.807f * fixedDeltaTime;

	state->objVelVector += (state->objAccelVector * fixedDeltaTime) - (state->objVelVector.inverse() / 0.99f);

	// pos(new) = pos(now) + (h / 6) * (k1 + (2 * k2) + (2 * k3) + k4)
	// 
	// where
	// 
	// k1 = f(0, pos(now))
	// k2 = f(h / 2, pos(now) + (h / (k1 / 2)))
	// k3 = f(h / 2, pos(now) + (h / (k2 / 2)))
	// k4 = f(h, pos(now) + (h * k3))
	// 
	// f(h, p) = p + (vel * h)
	// 
	// so
	// 
	// k1 = pos(now)
	// k2 = (pos(now) + (h / (k1 / 2))) + (vel * h / 2)
	// k3 = pos(now) + (h / (k2 / 2)) + (vel * h / 2)
	// k4 = pos(now) + (h * k3) + (vel * h)
	// 
	// Can't do h / LargeVector, so flip all of those around
	// k1 = pos(now)
	// k2 = (pos(now) + ((1 / (k1 / 2)) * h)) + (vel * h / 2)
	// k3 = pos(now) + ((1 / (k2 / 2)) * h) + (vel * h / 2)
	// k4 = pos(now) + (k3 * h) + (vel * h)
	// 
	// h = fixedDeltaTime
	// pos = state->objStateVector
	// vel = state->objVelVector

	// k1 is removed as it simplifies to state->objStateVector, so there's no reason to have it as a seperate variable
	k2 = (state->objStateVector + ((state->objStateVector / 2).inverse() * fixedDeltaTime)) + (state->objVelVector * (fixedDeltaTime / 2));
	k3 = state->objStateVector + ((k2 / 2).inverse() * fixedDeltaTime) + (state->objVelVector * (fixedDeltaTime / 2));
	k4 = state->objStateVector + (k3 * fixedDeltaTime) + (state->objVelVector * fixedDeltaTime);

	state->objStateVector += (state->objStateVector + (k2 * 2) + (k3 * 2) + k4) * (fixedDeltaTime / 6);
}