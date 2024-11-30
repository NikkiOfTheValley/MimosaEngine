#include "resource_manager.h"
#include <resource/texture_manager.h>

void ResourceManager::LoadTexture(std::string path, bool isRGBA, std::string name)
{
	logger.log("Loading texture " + name + " (at " + path + ")");

	TextureManager::getInstance().AddTexture(path, name, isRGBA);
	
}

void ResourceManager::LoadShader(std::string vertPath, std::string fragPath, std::string name)
{
	logger.log("Loading shader " + name + " (at " + vertPath + " and " + fragPath + ")");
	nameToShaderIndex[name] = shaders.size();
	shaders.push_back(new Shader(vertPath, fragPath));
}

void ResourceManager::LoadTexture(const unsigned char* data, size_t len, bool isRGBA, std::string name)
{
	logger.log("Loading texture " + name + " (embedded resource)");

	Image* img = new Image(data, len, isRGBA);

	TextureManager::getInstance().AddImage(img, name);
}

void ResourceManager::LoadShader(const unsigned char* vertData, size_t vertLength, const unsigned char* fragData, size_t fragLength, std::string name)
{
	logger.log("Loading shader " + name + " (embedded resource)");
	nameToShaderIndex[name] = shaders.size();
	shaders.push_back(new Shader(vertData, vertLength, fragData, fragLength));
}

std::shared_ptr<Texture> ResourceManager::GetTexture(std::string name)
{
	return TextureManager::getInstance().GetTexture(name);
}

Shader* ResourceManager::GetShader(std::string name)
{
	if (!nameToShaderIndex.contains(name))
	{
		logger.err("No shader exists named " + name);
		return nullptr;
	}

	return shaders[nameToShaderIndex[name]];
}

void ResourceManager::ReloadTextures()
{
	// Not implemented
	logger.warn("ResourceManager::ReloadTextures() is not implemented!");
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