#pragma once
#include "../rendering/types/vec.h"
#include <vector>

// Prefefine Mesh and PhysObj so we don't have to `include` them
class Mesh;
class PhysObj;

struct collision_vert
{
	vec3 pos;
	vec3 normal;
};

class CollisionMesh
{
public:
	// The OBJ needs to be in the format of a bunch of convex "blocks" if the original model is concave
	void LoadFromOBJ(std::string path);

	// This function is very slow for large blocks, don't use it in preformace-critical locations!
	void AddBlock(const Mesh& mesh);

	const std::vector<std::vector<collision_vert>>& GetBlocks();

private:
	std::vector<std::vector<collision_vert>> convexBlocks;
};