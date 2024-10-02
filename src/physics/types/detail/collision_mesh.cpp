#include "../collision_mesh.h"
#include "../assert.h"
#include "rendering/types/mesh.h"
#include "rendering/types/obj_loader.h"

void CollisionMesh::AddPrimitive(CollisionPrimitive primitive)
{
	primitives.push_back(primitive);
}

void CollisionMesh::LoadFromFile(std::string path)
{

}

const std::vector<CollisionPrimitive> CollisionMesh::GetPrimitives()
{
	return this->primitives;
}