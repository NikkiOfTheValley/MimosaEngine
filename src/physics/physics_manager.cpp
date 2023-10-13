#include "physics_manager.h"
#include "../core/accurate_timer.h"

void PhysicsManager::Init()
{
	
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
				Logger::getInstance().log("deltaTime: " + std::to_string(deltaTime * 1000) + "ms | stepTime: " + std::to_string(stepTime * 1000000000) + "ns | SPS: " + std::to_string(((int)ceil(1.f / deltaTime))));
				timer = 0;
				Logger::getInstance().log(GetPhysObject("ground")->GetProperties().pos);
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

	state.isInStep = false;
}

void PhysicsManager::CreateObject(std::string name, vec3 pos, vec3 rot, float mass, CollisionConstraint* collisionConstraint, std::vector<Constraint*> constraints, vec3 vel, vec3 angVel, bool hasGravity, bool isPinned)
{
	PhysObj obj = PhysObj(&state, state.objIndex, hasGravity);

	state.objVelVector[state.objIndex * 6] = vel.data[0];
	state.objVelVector[(state.objIndex * 6) + 1] = vel.data[1];
	state.objVelVector[(state.objIndex * 6) + 2] = vel.data[2];

	state.objVelVector[(state.objIndex * 6) + 3] = angVel.data[0];
	state.objVelVector[(state.objIndex * 6) + 4] = angVel.data[1];
	state.objVelVector[(state.objIndex * 6) + 5] = angVel.data[2];

	obj.hasGravity = hasGravity;
	state.nameToObjIndex[name] = state.objIndex;

	// Set position
	state.objStateVector[state.objIndex * 6] = pos.data[0];
	state.objStateVector[(state.objIndex * 6) + 1] = pos.data[1];
	state.objStateVector[(state.objIndex * 6) + 2] = pos.data[2];

	// Set rotation
	state.objStateVector[(state.objIndex * 6) + 3] = rot.data[0];
	state.objStateVector[(state.objIndex * 6) + 4] = rot.data[1];
	state.objStateVector[(state.objIndex * 6) + 5] = rot.data[2];

	state.objPropertiesMatrix.data[state.objIndex * 3][state.objIndex * 3] = mass;
	state.objPropertiesMatrix.data[(state.objIndex * 3) + 1][(state.objIndex * 3) + 1] = mass;

	// Setting moment of inertia here makes no sense, as that depends
	// on where the force is being applied, which is an unknown in this function

	size_t endConstraintIndex = state.objConstraints[state.objIndex].second;

	// std::pair will initialize endConstraintIndex to zero on construction,
	// so we don't need to check if it's not initialized
	state.objConstraints[state.objIndex].first[endConstraintIndex] = collisionConstraint;

	// Increment the endConstraintIndex, as a constraint was just added
	state.objConstraints[state.objIndex].second++;

	for (auto& constraint : constraints)
	{
		state.objConstraints[state.objIndex].first[endConstraintIndex] = constraint;

		// Increment the endConstraintIndex, as a constraint was just added
		state.objConstraints[state.objIndex].second++;
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