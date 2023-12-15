#pragma once
#include "types/phys_state.h"
#include "phys_globals.h"
#include <thread>

class PhysicsManager
{
public:
	PhysicsManager();

	// Starts the simulation
	void Start();

	void CreateObject(std::string name, vec3 pos, vec3 rot, float density, CollisionMesh& collisionMesh, std::vector<Constraint*> constraints = {}, std::function<void(double /* fixedDeltaTime */)> updateFunc = nullptr, vec3 vel = vec3(), vec3 angVel = vec3(), bool hasGravity = true, bool isPinned = false);

	PhysObj* GetPhysObject(std::string name);

	void ApplyForce(std::string name, vec3 force);

	void ApplyTorque(std::string name, vec3 torque);

	bool runPhys = true;
private:
	void Step(double deltaTime);

	// Helper function for PhysicsManager::CreateObject(). Calculates the inertia tensor and mass for the current object.
	void CalculatePhysicalProperties(float density, const std::vector<collision_vert> verts);

	PhysState state;
	std::thread physThread;

	MLCPSolver mlcpSolver;
	SLESolver sleSolver;
	ODESolver odeSolver;
	CollisionDetector collisionDetector;

	unsigned int timer = 0;
};