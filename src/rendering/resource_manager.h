#pragma once
#include "rendering/types/texture.h"
#include "rendering/types/shader.h"
#include <unordered_map>

// The ResourceManager is a Singleton because more than one thing accesses it while also needing global state

class ResourceManager
{
public:
	// Should be thread-safe, magic statics (also known as "dynamic initialization and destruction with concurrency") were implemented in VS 2015
	static ResourceManager& getInstance()
	{
		static ResourceManager instance;
		return instance;
	}

	void LoadTexture(std::string path, bool isRGBA, bool useNearestNeighbor, std::string name);
	void LoadShader(std::string vertPath, std::string fragPath, std::string name);
	void LoadModel(std::string path, std::string name);

	std::shared_ptr<Texture> GetTexture(std::string name);
	Shader* GetShader(std::string name);

	void ReloadTextures();
	void ReloadShaders();

	// Deallocates all shaders and textures, this must be called before the ResourceManager goes out of scope
	void Dealloc();

	ResourceManager(ResourceManager const&) = delete;
	void operator=(ResourceManager const&) = delete;

private:
	ResourceManager() {};

	std::vector<Shader*> shaders;
	std::unordered_map<std::string, size_t> nameToShaderIndex;

	std::vector<std::shared_ptr<Texture>> textures;
	std::unordered_map<std::string, size_t> nameToTextureIndex;
};