#include "resource_manager.h"
#include <resource/texture_manager.h>

void ResourceManager::LoadTexture(ResourceReference resource, bool isRGBA, std::string name)
{
	logger.log("Loading texture " + name);
	resource_data data = resource.GetData();

	Image* img = new Image(data.data, data.length, isRGBA);

	TextureManager::getInstance().AddImage(img, name);
}

void ResourceManager::LoadShader(ResourceReference vertResource, ResourceReference fragResource, std::string name)
{
	logger.log("Loading shader " + name);
	nameToShaderIndex[name] = shaders.size();

	resource_data vertData = vertResource.GetData();
	resource_data fragData = fragResource.GetData();

	if (vertData.data == nullptr && fragData.data == nullptr)
	{
		Shader* shader = new Shader();
		shader->GenShaderFromStrings(vertData.dataAsText, fragData.dataAsText);

		shaders.push_back(shader);
		return;
	}
	else if (vertData.data != nullptr && fragData.data != nullptr)
	{
		shaders.push_back(new Shader(vertData.data, vertData.length, fragData.data, fragData.length));
		return;
	}

	logger.err("ResourceManager::LoadShader was given invalid resource references!");
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