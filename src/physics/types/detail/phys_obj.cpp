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
	phys_obj_prop prop;
	
	prop.pos.x = state->objStateVector[index * 6];
	prop.pos.y = state->objStateVector[(index * 6) + 1];
	prop.pos.z = state->objStateVector[(index * 6) + 2];

	prop.rot.x = state->objStateVector[(index * 6) + 3];
	prop.rot.y = state->objStateVector[(index * 6) + 4];
	prop.rot.z = state->objStateVector[(index * 6) + 5];

	prop.accel.x = state->objAccelVector[(index * 6)];
	prop.accel.y = state->objAccelVector[(index * 6) + 1];
	prop.accel.z = state->objAccelVector[(index * 6) + 2];

	prop.angAccel.x = state->objAccelVector[(index * 6) + 3];
	prop.angAccel.y = state->objAccelVector[(index * 6) + 4];
	prop.angAccel.z = state->objAccelVector[(index * 6) + 5];

	prop.vel.x = state->objVelVector[(index * 6)];
	prop.vel.y = state->objVelVector[(index * 6) + 1];
	prop.vel.z = state->objVelVector[(index * 6) + 2];

	prop.angVel.x = state->objVelVector[(index * 6) + 3];
	prop.angVel.y = state->objVelVector[(index * 6) + 4];
	prop.angVel.z = state->objVelVector[(index * 6) + 5];

	prop.mass = state->objPropertiesMatrix[{index * 3, index * 3}];

	return prop;
}

void PhysObj::SetProperties(phys_obj_prop prop)
{
	state->objStateVector[index * 6] = prop.pos.x;
	state->objStateVector[(index * 6) + 1] = prop.pos.y;
	state->objStateVector[(index * 6) + 2] = prop.pos.z;

	state->objStateVector[(index * 6) + 3] = prop.rot.x;
	state->objStateVector[(index * 6) + 4] = prop.rot.y;
	state->objStateVector[(index * 6) + 5] = prop.rot.z;

	state->objAccelVector[(index * 6)] = prop.accel.x;
	state->objAccelVector[(index * 6) + 1] = prop.accel.y;
	state->objAccelVector[(index * 6) + 2] = prop.accel.z;

	state->objAccelVector[(index * 6) + 3] = prop.angAccel.x;
	state->objAccelVector[(index * 6) + 4] = prop.angAccel.y;
	state->objAccelVector[(index * 6) + 5] = prop.angAccel.z;

	state->objVelVector[(index * 6)] = prop.vel.x;
	state->objVelVector[(index * 6) + 1] = prop.vel.y;
	state->objVelVector[(index * 6) + 2] = prop.vel.z;

	state->objVelVector[(index * 6) + 3] = prop.angVel.x;
	state->objVelVector[(index * 6) + 4] = prop.angVel.y;
	state->objVelVector[(index * 6) + 5] = prop.angVel.z;

	state->objPropertiesMatrix[{index * 3, index * 3}] = prop.mass;
}