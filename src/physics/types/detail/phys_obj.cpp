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
	
	prop.pos.x = state->objPositionVector[index * 3];
	prop.pos.y = state->objPositionVector[(index * 3) + 1];
	prop.pos.z = state->objPositionVector[(index * 3) + 2];

	prop.rot.x = state->objRotationVector[index * 3];
	prop.rot.y = state->objRotationVector[(index * 3) + 1];
	prop.rot.z = state->objRotationVector[(index * 3) + 2];

	prop.accel.x = state->objAccelVector[(index * 3)];
	prop.accel.y = state->objAccelVector[(index * 3) + 1];
	prop.accel.z = state->objAccelVector[(index * 3) + 2];

	prop.angAccel.x = state->objAngAccelVector[(index * 3)];
	prop.angAccel.y = state->objAngAccelVector[(index * 3) + 1];
	prop.angAccel.z = state->objAngAccelVector[(index * 3) + 2];

	prop.vel.x = state->objVelVector[(index * 3)];
	prop.vel.y = state->objVelVector[(index * 3) + 1];
	prop.vel.z = state->objVelVector[(index * 3) + 2];

	prop.angVel.x = state->objAngVelVector[(index * 3)];
	prop.angVel.y = state->objAngVelVector[(index * 3) + 1];
	prop.angVel.z = state->objAngVelVector[(index * 3) + 2];

	prop.mass = state->objMass[index * 3];

	return prop;
}

void PhysObj::SetProperties(phys_obj_prop prop)
{
	state->objPositionVector[index * 3] = prop.pos.x;
	state->objPositionVector[(index * 3) + 1] = prop.pos.y;
	state->objPositionVector[(index * 3) + 2] = prop.pos.z;

	state->objRotationVector[index * 3] = prop.rot.x;
	state->objRotationVector[(index * 3) + 1] = prop.rot.y;
	state->objRotationVector[(index * 3) + 2] = prop.rot.z;

	state->objAccelVector[index * 3] = prop.accel.x;
	state->objAccelVector[(index * 3) + 1] = prop.accel.y;
	state->objAccelVector[(index * 3) + 2] = prop.accel.z;

	state->objAngAccelVector[index * 3] = prop.angAccel.x;
	state->objAngAccelVector[(index * 3) + 1] = prop.angAccel.y;
	state->objAngAccelVector[(index * 3) + 2] = prop.angAccel.z;

	state->objVelVector[index * 3] = prop.vel.x;
	state->objVelVector[(index * 3) + 1] = prop.vel.y;
	state->objVelVector[(index * 3) + 2] = prop.vel.z;

	state->objAngVelVector[index * 3] = prop.angVel.x;
	state->objAngVelVector[(index * 3) + 1] = prop.angVel.y;
	state->objAngVelVector[(index * 3) + 2] = prop.angVel.z;

	state->objMass[index * 3] = prop.mass;
}