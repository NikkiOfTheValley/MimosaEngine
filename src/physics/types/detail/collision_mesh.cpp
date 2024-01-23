#include "../collision_mesh.h"
#include "../assert.h"
#include "rendering/types/mesh.h"
#include "rendering/types/obj_loader.h"

// The OBJ needs to be in the format of a bunch of convex "blocks" if the original model is concave
void CollisionMesh::LoadFromOBJ(std::string path)
{
	std::vector<std::vector<vert>> objects = obj_loader::LoadOBJ(path);

	// Convert objects to blocks
	for (auto& obj : objects)
	{
		std::vector<collision_vert> block;

		for (auto& vert : obj)
		{
			collision_vert collisionVert;

			collisionVert.pos.x = vert.x;
			collisionVert.pos.y = vert.y;
			collisionVert.pos.z = vert.z;
			collisionVert.normal.x = vert.normX;
			collisionVert.normal.y = vert.normY;
			collisionVert.normal.z = vert.normZ;

			block.push_back(collisionVert);
		}

		convexBlocks.push_back(block);
	}
}

void CollisionMesh::AddBlock(const Mesh& mesh)
{
	std::vector<collision_vert> block;

	// Resize the new block to the space required for the Mesh's vertices, so the vector doesn't reallocate a bunch of times
	block.resize(mesh.verts.size());

	for (size_t i = 0; i < mesh.verts.size(); i++)
	{
		// Cache the mesh vert, so it doesn't have to be accessed a bunch of times (also a bit easier to read)
		vert meshVert = mesh.verts[i];
		collision_vert collisionVert;

		collisionVert.pos.x = meshVert.x;
		collisionVert.pos.y = meshVert.y;
		collisionVert.pos.z = meshVert.z;
		collisionVert.normal.x = meshVert.normX;
		collisionVert.normal.y = meshVert.normY;
		collisionVert.normal.z = meshVert.normZ;

		block[i] = collisionVert;
	}

	convexBlocks.push_back(block);
}

const std::vector<std::vector<collision_vert>>& CollisionMesh::GetBlocks()
{
	return this->convexBlocks;
}