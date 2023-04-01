#pragma once
#include <vector>
#include <chrono>
#include "mesh.h"

namespace obj_loader
{
	std::vector<vert> LoadOBJ(std::string path);
}