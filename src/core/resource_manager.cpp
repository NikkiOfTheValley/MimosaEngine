#include "resource_manager.h"

void ResourceManager::LoadTexture(std::string path, bool isRGBA, bool useNearestNeighbor, std::string name)
{
	logger->log("Loading texture " + name + " (at " + path + ")");
	nameToTexIndex[name] = textures.size();
	textures.push_back(new Texture(path, isRGBA, useNearestNeighbor));
}

void ResourceManager::LoadShader(std::string vertPath, std::string fragPath, std::string name)
{
	logger->log("Loading shader " + name + " (at " + vertPath + " and " + fragPath + ")");
	nameToShaderIndex[name] = shaders.size();
	shaders.push_back(new Shader(vertPath, fragPath));
}

Texture* ResourceManager::GetTexture(std::string name)
{
	if (!nameToTexIndex.contains(name))
	{
		logger->err("No texture exists called " + name);
		return nullptr;
	}

	return textures[nameToTexIndex[name]];
}

Shader* ResourceManager::GetShader(std::string name)
{
	if (!nameToShaderIndex.contains(name))
	{
		logger->err("No shader exists called " + name);
		return nullptr;
	}

	return shaders[nameToShaderIndex[name]];
}

void ResourceManager::ReloadTextures()
{
	for (auto& t : nameToTexIndex)
	{
		std::string path = textures[t.second]->path;
		bool isRGBA = textures[t.second]->IsRGBA();
		bool useNearestNeighbor = textures[t.second]->useNearestNeighbor;

		delete textures[t.second];
		textures[t.second] = new Texture(path, isRGBA, useNearestNeighbor);
	}
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
	for (auto& t : nameToTexIndex)
	{
		delete textures[t.second];
	}

	for (auto& s : nameToShaderIndex)
	{
		delete shaders[s.second];
	}
}