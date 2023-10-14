#include "../phys_obj.h"
#include "../phys_state.h"
#include <intrin.h>

void PhysObj::Update(double fixedDeltaTime)
{
	if (updateFunc != nullptr)
		updateFunc(fixedDeltaTime);
}

phys_obj_prop PhysObj::GetProperties()
{
	// Just spin until the step has completed
	//while (state->isInStep)
	//	__nop();

	phys_obj_prop prop;
	
	prop.pos.data[0] = state->objStateVector[index * 6];
	prop.pos.data[1] = state->objStateVector[(index * 6) + 1];
	prop.pos.data[2] = state->objStateVector[(index * 6) + 2];

	prop.rot.data[0] = state->objStateVector[(index * 6) + 3];
	prop.rot.data[1] = state->objStateVector[(index * 6) + 4];
	prop.rot.data[2] = state->objStateVector[(index * 6) + 5];

	prop.accel.data[0] = state->objAccelVector[(index * 6)];
	prop.accel.data[1] = state->objAccelVector[(index * 6) + 1];
	prop.accel.data[2] = state->objAccelVector[(index * 6) + 2];

	prop.angAccel.data[0] = state->objAccelVector[(index * 6) + 3];
	prop.angAccel.data[1] = state->objAccelVector[(index * 6) + 4];
	prop.angAccel.data[2] = state->objAccelVector[(index * 6) + 5];

	prop.vel.data[0] = state->objVelVector[(index * 6)];
	prop.vel.data[1] = state->objVelVector[(index * 6) + 1];
	prop.vel.data[2] = state->objVelVector[(index * 6) + 2];

	prop.angVel.data[0] = state->objVelVector[(index * 6) + 3];
	prop.angVel.data[1] = state->objVelVector[(index * 6) + 4];
	prop.angVel.data[2] = state->objVelVector[(index * 6) + 5];

	prop.mass = state->objPropertiesMatrix.data[index * 3][index * 3];

	return prop;
}

void PhysObj::SetProperties(phys_obj_prop prop)
{
	// Just spin until the step has completed
	while (state->isInStep)
		__nop();

	state->objStateVector[index * 6] = prop.pos.data[0];
	state->objStateVector[(index * 6) + 1] = prop.pos.data[1];
	state->objStateVector[(index * 6) + 2] = prop.pos.data[2];

	state->objStateVector[(index * 6) + 3] = prop.rot.data[0];
	state->objStateVector[(index * 6) + 4] = prop.rot.data[1];
	state->objStateVector[(index * 6) + 5] = prop.rot.data[2];

	state->objAccelVector[(index * 6)] = prop.accel.data[0];
	state->objAccelVector[(index * 6) + 1] = prop.accel.data[1];
	state->objAccelVector[(index * 6) + 2] = prop.accel.data[2];

	state->objAccelVector[(index * 6) + 3] = prop.angAccel.data[0];
	state->objAccelVector[(index * 6) + 4] = prop.angAccel.data[1];
	state->objAccelVector[(index * 6) + 5] = prop.angAccel.data[2];

	state->objVelVector[(index * 6)] = prop.vel.data[0];
	state->objVelVector[(index * 6) + 1] = prop.vel.data[1];
	state->objVelVector[(index * 6) + 2] = prop.vel.data[2];

	state->objVelVector[(index * 6) + 3] = prop.angVel.data[0];
	state->objVelVector[(index * 6) + 4] = prop.angVel.data[1];
	state->objVelVector[(index * 6) + 5] = prop.angVel.data[2];

	state->objPropertiesMatrix.data[index * 3][index * 3] = prop.mass;
}