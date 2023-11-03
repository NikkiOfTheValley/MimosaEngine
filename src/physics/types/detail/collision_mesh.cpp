#include "../collision_mesh.h"
#include "../assert.h"
#include "../rendering/types/mesh.h"

// This operator is very slow for large meshes, don't use it in preformace-critical locations!
void CollisionMesh::operator=(const Mesh& rhs)
{
	// Resize to the space required for the Mesh's vertices, so the vector doesn't reallocate a bunch of times
	this->verts.resize(rhs.verts.size());

	for (size_t i = 0; i < rhs.verts.size(); i++)
	{
		// Cache the mesh vert, so it doesn't have to be accessed a bunch of times (also a bit easier to read)
		vert meshVert = rhs.verts[i];
		collision_vert collisionVert;

		collisionVert.pos.x = meshVert.x;
		collisionVert.pos.y = meshVert.y;
		collisionVert.pos.z = meshVert.z;
		collisionVert.normal.x = meshVert.normX;
		collisionVert.normal.y = meshVert.normY;
		collisionVert.normal.z = meshVert.normZ;
		
		this->verts[i] = collisionVert;
	}
}

const std::vector<collision_vert> CollisionMesh::GetVerts()
{
	return this->verts;
}