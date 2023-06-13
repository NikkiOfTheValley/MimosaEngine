#include "resource_manager.h"
#include "../rendering/texture_manager.h"

void ResourceManager::LoadTexture(std::string path, bool isRGBA, bool useNearestNeighbor, std::string name)
{
	Logger::getInstance().log("Loading texture " + name + " (at " + path + ")");

	//TextureManager::getInstance().AddTexture(path, name, isRGBA, useNearestNeighbor);

	nameToTexIndex[name] = textures.size();
	textures.push_back(new Texture(path, isRGBA, useNearestNeighbor));
}

void ResourceManager::LoadShader(std::string vertPath, std::string fragPath, std::string name)
{
	Logger::getInstance().log("Loading shader " + name + " (at " + vertPath + " and " + fragPath + ")");
	nameToShaderIndex[name] = shaders.size();
	shaders.push_back(new Shader(vertPath, fragPath));
}

Texture* ResourceManager::GetTexture(std::string name)
{
	//TextureManager::getInstance().GetTextureLocation(name);


	if (!nameToTexIndex.contains(name))
	{
		Logger::getInstance().err("No texture exists named " + name);
		return nullptr;
	}

	return textures[nameToTexIndex[name]];
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