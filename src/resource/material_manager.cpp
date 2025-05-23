#include "material_manager.h"

void MaterialManager::CreateMaterial(
	std::string name,
	std::string shaderName,
	std::string textureName,
	std::vector<std::pair<std::any /* DEFAULT VALUE */, std::string /* NAME */>> customUniforms)
{
	logger.log("Creating material " + name);
	nameToMaterialIndex[name] = materials.size();
	materialIndexToName[materials.size()] = name;
	materials.push_back(new Material(shaderName, textureName, customUniforms));
}

Material* MaterialManager::GetMaterial(std::string name)
{
	if (!nameToMaterialIndex.contains(name))
	{
		logger.err("No material exists called " + name);
		return nullptr;
	}

	return materials[nameToMaterialIndex[name]];
}

void MaterialManager::ReloadMaterials()
{
	size_t i = 0;
	for (auto& material : materials)
	{
		logger.log("Reloading material " + materialIndexToName[i]);
		material->Reload();
		i++;
	}
		
}