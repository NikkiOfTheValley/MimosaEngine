#pragma once
#include <vector>
#include <chrono>
#include "mesh.h"

namespace obj
{
	// Returns a vector of the meshes in the given OBJ file
	std::vector<std::vector<vert>> LoadOBJ(std::string path);
}