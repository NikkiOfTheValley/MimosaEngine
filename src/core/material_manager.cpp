#include "material_manager.h"

void MaterialManager::CreateMaterial(
	std::string name,
	std::string shaderName,
	std::string textureName,
	std::vector<std::pair<std::any /* DEFAULT VALUE */, std::string /* NAME */>> customUniforms)
{
	Logger::getInstance().log("Creating material " + name);
	nameToMaterialIndex[name] = materials.size();
	materials.push_back(new Material(shaderName, textureName, customUniforms));
}

Material* MaterialManager::GetMaterial(std::string name)
{
	if (!nameToMaterialIndex.contains(name))
	{
		Logger::getInstance().err("No material exists called " + name);
		return nullptr;
	}

	return materials[nameToMaterialIndex[name]];
}

void MaterialManager::ReloadMaterials()
{
	for (auto& material : materials)
		material->Reload();
}