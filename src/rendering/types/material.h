#pragma once
#include "shader.h"
#include "texture.h"

class Material
{
public:
	Material(
		std::string shaderName,
		std::string textureName,
		std::vector<std::pair<std::any /* DEFAULT VALUE */, std::string /* NAME */>> customUniforms = {});

	void SetUniform(std::string name, std::any value);

	void Bind(mat4x4f viewMatrix, mat4x4f projectionMatrix, mat4x4f modelMatrix);

	void Reload();

	// Registers a new mesh that uses this material
	void RegisterNewMesh(std::string meshName);

private:
	Shader* shader;
	std::string shaderName;

	Texture* texture;
	std::string textureName;

	std::vector<std::pair<std::any /* VALUE */, std::string /* NAME */>> customUniforms;
	std::vector<std::string> meshesThatUseThisMaterial;
};