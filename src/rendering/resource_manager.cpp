#include "resource_manager.h"
#include "rendering/texture_manager.h"

void ResourceManager::LoadTexture(std::string path, bool isRGBA, bool useNearestNeighbor, std::string name)
{
	Logger::getInstance().log("Loading texture " + name + " (at " + path + ")");

	TextureManager::getInstance().AddTexture(path, name, isRGBA, useNearestNeighbor);
}

void ResourceManager::LoadShader(std::string vertPath, std::string fragPath, std::string name)
{
	Logger::getInstance().log("Loading shader " + name + " (at " + vertPath + " and " + fragPath + ")");
	nameToShaderIndex[name] = shaders.size();
	shaders.push_back(new Shader(vertPath, fragPath));
}

std::shared_ptr<Texture> ResourceManager::GetTexture(std::string name)
{
	std::shared_ptr<Texture> tex = std::make_shared<Texture>(TextureManager::getInstance().GetTextureLocation(name));
	return tex;
}

Shader* ResourceManager::GetShader(std::string name)
{
	if (!nameToShaderIndex.contains(name))
	{
		Logger::getInstance().err("No shader exists named " + name);
		return nullptr;
	}

	return shaders[nameToShaderIndex[name]];
}

void ResourceManager::ReloadTextures()
{
	// Not implemented
}

void ResourceManager::ReloadShaders()
{
	for (auto& s : nameToShaderIndex)
	{
		std::string vertPath = shaders[s.second]->vertPath;
		std::string fragPath = shaders[s.second]->fragPath;

		delete shaders[s.second];
		shaders[s.second] = new Shader(vertPath, fragPath);
	}
}

void ResourceManager::Dealloc()
{
	for (auto& s : nameToShaderIndex)
	{
		delete shaders[s.second];
	}
}