#include "physics_manager.h"
#include "core/accurate_timer.h"
#include <math/formatting_util.h>
#include <math/conversion.h>

using namespace math;

PhysicsManager::PhysicsManager()
{
	for (size_t i = 0; i < (MAX_PHYS_OBJECTS * MAX_CONSTRAINTS_PER_PHYS_OBJ) - 1; i++)
		state.objConstraints[i] = nullptr;
}

void PhysicsManager::Start()
{
	runPhys = true;
	physThread = std::thread([=]() {

		while (runPhys)
		{
			using namespace std::chrono;

			auto startOfStep = steady_clock::now();

			float fixedDeltaTime = 1.f / (float)PHYS_FPS;

			// Large time steps (which would happen when the update rate is low) 
			// cause instability, so just assume the physics is running at full speed
			Step(fixedDeltaTime);

			auto endOfStep = steady_clock::now();
			auto stepTime = ((double)duration_cast<nanoseconds>(endOfStep - startOfStep).count() * conv::NANOSEC_TO_SEC);
			
			if (stepTime > fixedDeltaTime && WARN_ON_SIM_LAG)
				Logger::getInstance().warn("Simulation behind by " + std::to_string((stepTime - fixedDeltaTime) * conv::SEC_TO_NANOSEC) + "ns");

			BlockForNanoseconds((long long)((fixedDeltaTime - stepTime) * conv::SEC_TO_NANOSEC));
			

			if (timer == PHYS_FPS)
			{
				double totalStepTime = 0.0;
				double averageStepTime = 0.0;
				double worstStepTime = 0.0;

				for (auto& value : stepTimeOverLastSecond)
				{
					if (value > worstStepTime)
						worstStepTime = value;

					totalStepTime += value;
				}

				averageStepTime = totalStepTime / max((double)stepTimeOverLastSecond.size(), 1);

				Logger::getInstance().log(
					"fixedDeltaTime: " + math::floatToString((float)fixedDeltaTime * conv::SEC_TO_MICROSEC, 4) + "us "
					"|  stepTime: "+ math::floatToString((float)averageStepTime * conv::SEC_TO_MICROSEC, 4) + "us "
					"| avgUpdateRate: " + std::to_string(((int)ceil(1.f / (max(averageStepTime, fixedDeltaTime)) / 1000))) + "kHz "
					"| worstUpdateRate: " + std::to_string(((int)ceil(1.f / (max(worstStepTime, fixedDeltaTime)) / 1000))) + "kHz");
				timer = 0;
			}

			stepTimeOverLastSecond[timer] = stepTime;

			timer++;
		}
		});
}

void PhysicsManager::Stop()
{
	runPhys = false;
	physThread.join();
}

void PhysicsManager::Step(double fixedDeltaTime)
{
	state.isInStep = true;

	// Check for collisions
	collisionHandler.HandleCollisions(&state);

	// Run solver
	odeSolver.Solve(&state, (float)fixedDeltaTime,
		[](PhysState* /*initialState*/, obj_state& /*state*/, double /*fixedDeltaTime*/)
		{
			force_and_torque forceAndTorque;

			//forceAndTorque.forceVector -= (state.objVelVector * state.objVelVector) * DRAG_CONSTANT;

			return forceAndTorque;
		}, &sleSolver);

	// Zero out force vector
	state.objForceVector.data.fill(0.f);

	// Call the custom update functionality on the objects
	for (auto& obj : state.objects)
		if (obj.index <= MAX_PHYS_OBJECTS)
			obj.Update(fixedDeltaTime);

	// Calculate gravity
	for (auto& obj : state.objects)
		if (obj.hasGravity && obj.index <= MAX_PHYS_OBJECTS)
			state.objForceVector[(obj.index * 3) + 1] += (float)(-9.8f * state.objMass[(obj.index * 3) + 1] * (float)fixedDeltaTime);



	// Apply drag
	//state.objForceVector -= DRAG_CONSTANT * (float)fixedDeltaTime;

	state.objVelVector = state.objVelVector - (state.objVelVector * 0.05f) - ((state.objVelVector * state.objVelVector) * 0.01f);

	state.isInStep = false;
}

void PhysicsManager::CreateObject(std::string name, vec3 pos, vec3 rot, float /*density*/, CollisionMesh& collisionMesh, std::vector<Constraint*> constraints, std::function<void(double /* fixedDeltaTime */)> updateFunc, vec3 vel, vec3 angVel, bool hasGravity, bool isPinned)
{
	if (state.objIndex >= MAX_PHYS_OBJECTS)
	{
		Logger::getInstance().err("Failed to create PhysObj because the number of objects is >= MAX_PHYS_OBJECTS");
		return;
	}

	// - Create the object and set various properties from the arguments -

	PhysObj obj = PhysObj(&state, state.objIndex, hasGravity, updateFunc);

	state.objVelVector[state.objIndex * 3] = vel.x;
	state.objVelVector[(state.objIndex * 3) + 1] = vel.y;
	state.objVelVector[(state.objIndex * 3) + 2] = vel.z;

	state.objAngVelVector[(state.objIndex * 3)] = angVel.x;
	state.objAngVelVector[(state.objIndex * 3) + 1] = angVel.y;
	state.objAngVelVector[(state.objIndex * 3) + 2] = angVel.z;

	obj.hasGravity = hasGravity;
	obj.isPinned = isPinned;
	state.nameToObjIndex[name] = state.objIndex;
	obj.collisionMesh = collisionMesh;

	state.objPositionVector[state.objIndex * 3] = pos.x;
	state.objPositionVector[(state.objIndex * 3) + 1] = pos.y;
	state.objPositionVector[(state.objIndex * 3) + 2] = pos.z;

	state.objRotationVector[state.objIndex * 3] = rot.x;
	state.objRotationVector[(state.objIndex * 3) + 1] = rot.y;
	state.objRotationVector[(state.objIndex * 3) + 2] = rot.z;

	// Calculate inertia tensor and mass for this object
	//CalculatePhysicalProperties(density, collisionMesh.GetBlocks()[0]);

	// Since objPropertiesMatrix was just modified by CalculatePhysicalProperties, we have to update its inverse
	//state.objPropertiesMatrixInverse = state.objPropertiesMatrix;
	//state.objPropertiesMatrixInverse.inverseDiagonal();

	size_t startingIndex = state.objIndex * (MAX_CONSTRAINTS_PER_PHYS_OBJ - 1);

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

void PhysicsManager::ApplyForce(std::string name, vec3 force)
{
	size_t index = GetPhysObject(name)->index * 3;
	state.objForceVector[index] += force.x;
	state.objForceVector[index + 1] += force.y;
	state.objForceVector[index + 2] += force.z;
}

void PhysicsManager::ApplyTorque(std::string name, vec3 /*torque*/)
{
	//GetPhysObject(name)->angAccel += angForce;
}

// This function is based on https://www.geometrictools.com/Documentation/PolyhedralMassProperties.pdfm, which is licensed under https://creativecommons.org/licenses/by/4.0/.
// This code is also modified from the original to make it more readable and to work with existing code.
inline static void CalculatePhysicalPropertiesHelper(float& w0, float& w1, float& w2, float& f1, float& f2, float& f3, float& g0, float& g1, float& g2)
{
	float temp0 = w0 + w1;
	float temp1 = w0 * w0;
	float temp2 = temp1 + w1 * temp0;
	f1 = temp0 + w2;
	f2 = temp2 + w2 * f1;
	f3 = w0 * temp1 + w1 * temp2 + w2 * f2;
	g0 = f2 + w0 * (f1 + w0);
	g1 = f2 + w1 * (f1 + w1);
	g2 = f2 + w2 * (f1 + w2);
}

void PhysicsManager::CalculatePhysicalProperties(float density, const std::vector<collision_vert> verts)
{
	// This function is based on https://www.geometrictools.com/Documentation/PolyhedralMassProperties.pdfm, which is licensed under https://creativecommons.org/licenses/by/4.0/.
	// This code is also modified from the original to make it more readable and to work with existing code.

	// 1, x, y, z, x^2, y^2, z^2, xy, yz, zx
	float integrals[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	// Since a triangle is 3 vertices, to iterate over them we simply go over every third vertice
	for (size_t i = 0; i < verts.size(); i += 3)
	{
		vec3 v2 = verts.data()[i].pos;
		vec3 v1 = verts.data()[i + 1].pos;
		vec3 v0 = verts.data()[i + 2].pos;

		// Calculate edges
		vec3 abc0 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
		vec3 abc1 = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };

		// Calculate cross product of edges
		float d0 = abc0.y * abc1.z - abc1.y * abc0.z;
		float d1 = abc1.x * abc0.z - abc0.x * abc1.z;
		float d2 = abc0.x * abc1.y - abc1.x * abc0.y;

		vec3 g0;
		vec3 g1;
		vec3 g2;

		vec3 f1;
		vec3 f2;
		vec3 f3;

		CalculatePhysicalPropertiesHelper(v0.x, v1.x, v2.x, f1.x, f2.x, f3.x, g0.x, g1.x, g2.x);
		CalculatePhysicalPropertiesHelper(v0.y, v1.y, v2.y, f1.y, f2.y, f3.y, g0.y, g1.y, g2.y);
		CalculatePhysicalPropertiesHelper(v0.z, v1.z, v2.z, f1.z, f2.z, f3.z, g0.z, g1.z, g2.z);

		// Update integrals
		integrals[0] += d0 * f1.x;
		integrals[1] += d0 * f2.x;
		integrals[2] += d1 * f2.y;
		integrals[3] += d2 * f2.z;
		integrals[4] += d0 * f3.x;
		integrals[5] += d1 * f3.y;
		integrals[6] += d2 * f3.z;
		integrals[7] += d0 * (v0.y * g0.x + v1.y * g1.x + v2.y * g2.x);
		integrals[8] += d1 * (v0.z * g0.y + v1.z * g1.y + v2.z * g2.y);
		integrals[9] += d2 * (v0.x * g0.z + v1.x * g1.z + v2.x * g2.z);
	}

	const float multTable[10] = { 1.f / 6.f, 1.f / 24.f, 1.f / 24.f, 1.f / 24.f, 1.f / 60.f, 1.f / 60.f, 1.f / 60.f, 1.f / 120.f, 1.f / 120.f, 1.f / 120.f };

	for (int i = 0; i < 10; i++)
		integrals[i] *= multTable[i];

	float volume = integrals[0];

	// Calculate center of mass
	vec3 COM = { integrals[1] / volume, integrals[2] / volume, integrals[3] / volume };

	// Calculate inertia tensor relative to COM and in a body-fixed frame

	float tensor_xx = integrals[5] + integrals[6] - volume * (COM.y * COM.y + COM.z * COM.z);
	float tensor_yy = integrals[4] + integrals[6] - volume * (COM.z * COM.z + COM.x * COM.x);
	float tensor_zz = integrals[4] + integrals[5] - volume * (COM.x * COM.x + COM.y * COM.y);
	float tensor_xy = -(integrals[7] - volume * COM.x * COM.y);
	float tensor_yz = -(integrals[8] - volume * COM.y * COM.z);
	float tensor_xz = -(integrals[9] - volume * COM.z * COM.x);

	// Set mass
	state.objMass[state.objIndex * 3] = volume * density;
	state.objMass[(state.objIndex * 3) + 1] = volume * density;
	state.objMass[(state.objIndex * 3) + 2] = volume * density;

	// Set inertia tensor
	state.objInertiaMatrices[state.objIndex].data[0][0] = tensor_xx * density;
	state.objInertiaMatrices[state.objIndex].data[1][1] = tensor_yy * density;
	state.objInertiaMatrices[state.objIndex].data[2][2] = tensor_zz * density;

	// Since an inertia tensor is symmetric, we need to set both (1, 2) and (2, 1) since we only calculated (1, 2)
	state.objInertiaMatrices[state.objIndex].data[0][1] = tensor_xy * density;
	state.objInertiaMatrices[state.objIndex].data[1][0] = tensor_xy * density;

	state.objInertiaMatrices[state.objIndex].data[1][2] = tensor_yz * density;
	state.objInertiaMatrices[state.objIndex].data[2][1] = tensor_yz * density;

	state.objInertiaMatrices[state.objIndex].data[0][2] = tensor_xz * density;
	state.objInertiaMatrices[state.objIndex].data[2][0] = tensor_xz * density;

	Logger::getInstance().log("Calculated values:\n      Mass: " + std::to_string(volume * density) + "kg\n      Volume: " + std::to_string(volume) + "m^3\n      Center Of Mass: " + (std::string)COM);
	
}