#include "../physics_manager.h"

void PhysicsManager::Init()
{

}

void PhysicsManager::CreateObject(std::string /*name*/, vec3 /*pos*/, vec3 /*rot*/, vec3 /*vel*/, vec3 /*angVel*/)
{

}

PhysObj* PhysicsManager::GetObj(std::string name)
{
	if (!state.nameToObjIndex.contains(name))
	{

		return nullptr;
	}
}

void PhysicsManager::ApplyForce(std::string /*name*/, vec3 /*force*/)
{

}

void PhysicsManager::ApplyAngularForce(std::string /*name*/, vec3 /*angForce*/)
{

}