#pragma once
#include "rendering/types/material.h"
#include <unordered_map>

class MaterialManager
{
public:
	void CreateMaterial(
		std::string name,
		std::string shaderName,
		std::string textureName,
		std::vector<std::pair<std::any /* DEFAULT VALUE */, std::string /* NAME */>> customUniforms = {});

	Material* GetMaterial(std::string name);

	void ReloadMaterials();

private:
	std::vector<Material*> materials;
	std::unordered_map<std::string, size_t> nameToMaterialIndex;
};