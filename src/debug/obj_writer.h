#pragma once
#include <math/vec.h>
#include <vector>

namespace obj
{
	void WriteOBJ(std::string path, std::vector<math::vec3> verts, std::vector<size_t> indices);

	void WriteOBJ(std::string path, std::vector<math::vec3> verts);
}