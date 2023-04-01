#pragma once
#include "../rendering/types/texture.h"
#include "../rendering/types/shader.h"
#include <unordered_map>

class ResourceManager
{
public:
	void LoadTexture(std::string path, bool isRGBA, bool useNearestNeighbor, std::string name);
	void LoadShader(std::string vertPath, std::string fragPath, std::string name);

	Texture* GetTexture(std::string name);
	Shader* GetShader(std::string name);

	void ReloadTextures();
	void ReloadShaders();

	// Deallocates all shaders and textures, this must be called before the ResourceManager goes out of scope
	void Dealloc();

private:
	std::vector<Texture*> textures;
	std::unordered_map<std::string, size_t> nameToTexIndex;

	std::vector<Shader*> shaders;
	std::unordered_map<std::string, size_t> nameToShaderIndex;
};