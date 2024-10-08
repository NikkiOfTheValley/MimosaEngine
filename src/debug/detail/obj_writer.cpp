#include "../obj_writer.h"
#include <filesystem>

using namespace math;

namespace obj
{
	namespace fs = std::filesystem;

	void WriteOBJ(std::string path, std::vector<math::vec3> verts, std::vector<size_t> indices)
	{
		Logger* logger = &Logger::getInstance();

		logger->log("Creating OBJ file " + path);

		if (indices.size() % 3 != 0)
		{
			logger->err("Failed to create OBJ file " + path + " because there are incomplete triangles");
			return;
		}
			

		// Create the OBJ file (binary is fastest)
		std::ofstream file(std::filesystem::absolute(path), std::ios::binary);

		// Make sure the file was actually opened
		if (!file) { logger->err("Failed to create OBJ file " + path); return; }

		std::string output;

		output.append("# File created by MimosaEngine " + ENG_VERSION_STR + "\n");

		for (auto& vert : verts)
			output.append("v " + std::to_string(vert.x) + " " + std::to_string(vert.y) + " " + std::to_string(vert.z) + "\n");

		for (size_t i = 0; i < indices.size(); i += 3)
			output.append("f " + std::to_string(indices[i] + 1) + " " + std::to_string(indices[i + 1] + 1) + " " + std::to_string(indices[i + 2] + 1) + "\n");

		file.write(output.c_str(), output.size());
		file.close();

		logger->log("Created OBJ file " + path);
	}

	void WriteOBJ(std::string path, std::vector<math::vec3> verts)
	{
		Logger* logger = &Logger::getInstance();

		logger->log("Creating OBJ file " + path);

		if (verts.size() % 3 != 0)
		{
			logger->err("Failed to create OBJ file " + path + " because there are incomplete triangles");
			return;
		}
			

		// Create the OBJ file (binary is fastest)
		std::ofstream file(std::filesystem::absolute(path), std::ios::binary);

		// Make sure the file was actually opened
		if (!file) { logger->err("Failed to create OBJ file " + path); return; }

		std::string output;

		for (auto& vert : verts)
			output.append("v " + std::to_string(vert.x) + " " + std::to_string(vert.y) + " " + std::to_string(vert.z) + "\n");

		for (size_t i = 0; i < verts.size(); i += 3)
			output.append("f " + std::to_string(i + 1) + " " + std::to_string(i + 2) + " " + std::to_string(i + 3) + "\n");

		file.write(output.c_str(), output.size());
		file.close();

		logger->log("Created OBJ file " + path);
	}
}
