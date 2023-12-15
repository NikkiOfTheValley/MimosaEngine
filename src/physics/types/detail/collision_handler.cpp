#include "../collision_handler.h"
#include "../phys_state.h"

void CollisionHandler::HandleCollisions(PhysState* state)
{
	for (auto& obj : state->objects)
	{
		collision collision;

		for (auto& block : obj.collisionMesh.GetBlocks())
		{


		}
	}
}