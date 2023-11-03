#include "physics_manager.h"
#include "../core/accurate_timer.h"

PhysicsManager::PhysicsManager()
{
	for (size_t i = 0; i < (MAX_PHYS_OBJECTS * MAX_CONSTRAINTS_PER_PHYS_OBJ) - 1; i++)
		state.objConstraints[i] = nullptr;
}

// Starts the simulation
void PhysicsManager::Start()
{
	physThread = std::thread([=]() {

		while (runPhys)
		{
			using namespace std::chrono;

			auto startOfStep = high_resolution_clock::now();

			// Large time steps (which would happen when the update rate is low) 
			// cause instability, so just assume the physics is running at full speed
			Step(1.f / (float)PHYS_FPS);

			auto endOfStep = high_resolution_clock::now();
			auto stepTime = ((double)duration_cast<nanoseconds>(endOfStep - startOfStep).count() / 1000000000);

			auto deltaTime = stepTime;

			if ((1.f / (float)PHYS_FPS) > stepTime)
				deltaTime += (1.f / (float)PHYS_FPS) - stepTime;

			BlockForNanoseconds((long long)(((1.f / (float)PHYS_FPS) - stepTime) * 1000000000));

			if (timer == PHYS_FPS)
			{
				Logger::getInstance().log("deltaTime: " + std::to_string(deltaTime * 1000) + "ms |  stepTime: " + std::to_string(stepTime * 1000) + "ms | SPS: " + std::to_string(((int)ceil(1.f / deltaTime))));
				timer = 0;
			}

			timer++;
		}
		});
}

void PhysicsManager::Step(double fixedDeltaTime)
{
	state.isInStep = true;

	// Solve equality constraints
	sleSolver.Solve(&state);

	// Run RK4
	odeSolver.Solve(&state, (float)fixedDeltaTime);

	// Call the custom update functionality on the objects last
	for (auto& obj : state.objects)
		obj.Update(fixedDeltaTime);

	// Calculate gravity
	for (auto& obj : state.objects)
		if (obj.hasGravity)
			state.objForceVector[(obj.index * 6) + 1] = -9.8f;

	state.objForceVector -= (state.objVelVector * state.objVelVector) * -DRAG_CONSTANT;

	state.isInStep = false;
}

void PhysicsManager::CreateObject(std::string name, vec3 pos, vec3 rot, float mass, CollisionConstraint* collisionConstraint, std::vector<Constraint*> constraints, vec3 vel, vec3 angVel, bool hasGravity, bool isPinned)
{
	if (state.objIndex >= MAX_PHYS_OBJECTS)
	{
		Logger::getInstance().err("Failed to create PhysObj because the number of objects is >= MAX_PHYS_OBJECTS");
		return;
	}

	// - Create the object and set various properties from the arguments -

	PhysObj obj = PhysObj(&state, state.objIndex, hasGravity);

	state.objVelVector[state.objIndex * 6] = vel.x;
	state.objVelVector[(state.objIndex * 6) + 1] = vel.y;
	state.objVelVector[(state.objIndex * 6) + 2] = vel.z;

	state.objVelVector[(state.objIndex * 6) + 3] = angVel.x;
	state.objVelVector[(state.objIndex * 6) + 4] = angVel.y;
	state.objVelVector[(state.objIndex * 6) + 5] = angVel.z;

	obj.hasGravity = hasGravity;
	state.nameToObjIndex[name] = state.objIndex;

	state.objStateVector[state.objIndex * 6] = pos.x;
	state.objStateVector[(state.objIndex * 6) + 1] = pos.y;
	state.objStateVector[(state.objIndex * 6) + 2] = pos.z;

	state.objStateVector[(state.objIndex * 6) + 3] = rot.x;
	state.objStateVector[(state.objIndex * 6) + 4] = rot.y;
	state.objStateVector[(state.objIndex * 6) + 5] = rot.z;

	state.objPropertiesMatrix.data[state.objIndex * 3][state.objIndex * 3] = mass;
	state.objPropertiesMatrix.data[(state.objIndex * 3) + 1][(state.objIndex * 3) + 1] = mass;

	// Setting moment of inertia here makes no sense, as that depends
	// on where the force is being applied, which is an unknown in this function


	size_t startingIndex = state.objIndex * (MAX_CONSTRAINTS_PER_PHYS_OBJ - 1);

	// The first constraint is always the collision constraint
	state.objConstraints[startingIndex] = collisionConstraint;

	for (auto constraint : constraints)
	{
		bool wasConstraintSet = false;

		for (size_t i = startingIndex; i < startingIndex + MAX_CONSTRAINTS_PER_PHYS_OBJ - 1; i++)
		{
			// Wait until we hit a constraint that isn't set, then use that "slot" for the new constraint
			if (!state.objConstraints[i])
			{
				state.objConstraints[i] = constraint;
				wasConstraintSet = true;
				break;
			}
		}

		if (!wasConstraintSet)
			Logger::getInstance().err("Failed to set constraint because the number of constraints is >= MAX_CONSTRAINTS_PER_PHYS_OBJ");
	}

	if (isPinned)
	{
		// TODO: Add pinning constraints here (or just don't add forces at all via a bool? Could make it more stable..)
	}

	state.objects[state.objIndex] = obj;
	state.objIndex++;
}

PhysObj* PhysicsManager::GetPhysObject(std::string name)
{
	if (!state.nameToObjIndex.contains(name))
	{
		Logger::getInstance().warn("No PhysObj named " + name + " exists.");
		return nullptr;
	}

	return &state.objects[state.nameToObjIndex[name]];
}

void PhysicsManager::ApplyForce(std::string name, vec3 /*force*/)
{
	//GetPhysObject(name)->accel += force;
}

void PhysicsManager::ApplyTorque(std::string name, vec3 /*torque*/)
{
	//GetPhysObject(name)->angAccel += angForce;
}