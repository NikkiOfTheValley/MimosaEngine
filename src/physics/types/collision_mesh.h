#pragma once
#include "../rendering/types/vec.h"
#include <vector>

// Prefefine Mesh so we don't have to `#include` it
class Mesh;

struct collision_vert
{
	vec3 pos;
	vec3 normal;
};

// Defines a given collision
struct collision
{
	
};

class CollisionMesh
{
public:
	// This operator is very slow for large meshes, don't use it in preformace-critical locations!
	void operator=(const Mesh& rhs);

	const std::vector<collision_vert> GetVerts();

private:

	std::vector<collision_vert> verts;
};