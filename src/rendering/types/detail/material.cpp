#include "../material.h"
#include "../../renderer.h"
#include "rendering/resource_manager.h"
#include "rendering/texture_manager.h"
#include "../mesh.h"

Material::Material(
	std::string shaderName,
	std::string textureName,
	std::vector<std::pair<std::any /* DEFAULT VALUE */, std::string /* NAME */>> customUniforms)
{
	shader = ResourceManager::getInstance().GetShader(shaderName);
	this->shaderName = shaderName;

	texture = ResourceManager::getInstance().GetTexture(textureName).get();
	this->textureName = textureName;

	this->customUniforms = customUniforms;
}

void Material::SetUniform(std::string name, std::any value)
{
	for (auto& uniform : customUniforms)
	{
		if (uniform.second == name)
		{
			uniform.first = value;
			return;
		}
	}

	Logger::getInstance().warn("No uniform named " + name + " found");
}

void Material::Bind(mat4x4f viewMatrix, mat4x4f projectionMatrix, mat4x4f modelMatrix)
{
	if (texture)
		texture->Bind(0);

	shader->Bind();
	shader->SetMat4("projection", projectionMatrix);
	shader->SetMat4("view", viewMatrix);
	shader->SetMat4("model", modelMatrix);

	if (texture)
	{
		shader->SetVec2("texSize", (float)texture->w, (float)texture->h);

		if (texture->IsAtlasTexture())
		{
			shader->SetVec2("atlasLocation", texture->GetLocation().first);
			shader->SetVec2("atlasSize", TextureManager::getInstance().GetAtlasDimensions());
		}
		else
		{
			shader->SetVec2("atlasLocation", -1, -1);
			shader->SetVec2("atlasSize", -1, -1);
		}
			
	}
	else
	{
		shader->SetVec2("texSize", -1, -1);
		shader->SetVec2("atlasLocation", -1, -1);
		shader->SetVec2("atlasSize", -1, -1);
	}
	

	for (auto& uniform : customUniforms)
		shader->SetAny(uniform.second, uniform.first);
}

void Material::Reload()
{
	shader = ResourceManager::getInstance().GetShader(shaderName);
	texture = ResourceManager::getInstance().GetTexture(textureName).get();

	for (auto& mesh : meshesThatUseThisMaterial)
		Renderer::getInstance().GetMesh(mesh)->UpdateMesh();
}

// Registers a new mesh that uses this material
void Material::RegisterNewMesh(std::string meshName)
{
	meshesThatUseThisMaterial.push_back(meshName);
}