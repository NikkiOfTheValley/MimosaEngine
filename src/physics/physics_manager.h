#pragma once
#include "types/phys_state.h"
#include "phys_globals.h"
#include <thread>

//! @brief Provides an API to interact with the physics engine
class PhysicsManager
{
public:
	PhysicsManager();

	//! @brief Starts the simulation
	void Start();

	//! @brief Stops the simulation
	void Stop();

	/*!
	 * @brief Creates a physics object
	 * @param name The name that will be used to refer to this object in the future
	 * @param pos The starting position of the object
	 * @param rot The starting rotation of the object
	 * @param density The density in kg/m^3 of the object
	 * @param collisionMesh The object's collision mesh
	 * @param constraints The constraints to put on the object
	 * @param updateFunc The function to call whenever this object is updated
	 * @param vel The starting velocity of the object
	 * @param angVel The starting angular velocity of the object
	 * @param hasGravity Set to true if the object is supposed to fall under gravity
	 * @param isPinned Set to true if the object is not supposed to move at all
	*/
	void CreateObject(std::string name, math::vec3 pos, math::vec3 rot, float density, CollisionMesh& collisionMesh, std::vector<Constraint*> constraints = {}, std::function<void(double /* fixedDeltaTime */)> updateFunc = nullptr, math::vec3 vel = math::vec3(), math::vec3 angVel = math::vec3(), bool hasGravity = true, bool isPinned = false);
	
	/*!
	 * @brief Gets a physics object
	 * @param name The name of the physics object to be retrieved
	 * @return The physics object, or nullptr if it couldn't be found
	*/
	PhysObj* GetPhysObject(std::string name);

	/*!
	 * @brief Applies a translational force to a physics object
	 * @param name The name of the physics object to apply the force to
	 * @param force The force to apply
	*/
	void ApplyForce(std::string name, math::vec3 force);

	/*!
	 * @brief Applies a torque to a physics object
	 * @param name The name of the physics object to apply the torque to
	 * @param torque The torque to apply
	*/
	void ApplyTorque(std::string name, math::vec3 torque);
private:
	/*!
	 * @brief Steps the physics simulation forwards
	 * @param fixedDeltaTime The time step to take in seconds. Should not be changed across steps, as this may cause instabilities.
	*/
	void Step(double fixedDeltaTime);

	//! @brief Helper function for PhysicsManager::CreateObject(). Calculates the inertia tensor and mass for the current object.
	void CalculatePhysicalProperties(float density, const std::vector<collision_vert> verts);

	PhysState state;
	std::thread physThread;
	bool runPhys = true;

	MLCPSolver mlcpSolver;
	SLESolver sleSolver;
	ODESolver odeSolver;
	CollisionHandler collisionHandler;

	unsigned int timer = 0;

	std::array<double, PHYS_FPS> stepTimeOverLastSecond;
};