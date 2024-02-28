#pragma once
#include <rendering/types/vec.h>
#include <vector>

namespace obj
{
	void WriteOBJ(std::string path, std::vector<vec3> verts, std::vector<size_t> indices);

	void WriteOBJ(std::string path, std::vector<vec3> verts);
}