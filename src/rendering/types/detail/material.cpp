#include "../material.h"
#include "../../renderer.h"
#include "rendering/resource_manager.h"
#include "rendering/texture_manager.h"
#include "../mesh.h"

using namespace math;

Material::Material(
	std::string shaderName,
	std::string textureName,
	std::vector<std::pair<std::any /* DEFAULT VALUE */, std::string /* NAME */>> customUniforms)
{
	shader = ResourceManager::getInstance().GetShader(shaderName);
	this->shaderName = shaderName;

	texture = ResourceManager::getInstance().GetTexture(textureName);
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

	logger.warn("No uniform named " + name + " found");
}

void Material::Bind(math::mat4x4f viewMatrix, math::mat4x4f projectionMatrix, math::mat4x4f modelMatrix)
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
	texture = ResourceManager::getInstance().GetTexture(textureName);

	// Keep track of any meshes that weren't found during the reload
	std::vector<std::string> meshesToDelete;
	for (auto& mesh : meshesThatUseThisMaterial)
	{
		Mesh* meshPointer = Renderer::getInstance().GetMesh(mesh);
		logger.log("Mesh " + mesh);
		if (!meshPointer)
		{
			logger.warn("Mesh " + mesh + " is a stale reference, deleting");
			meshesToDelete.push_back(mesh);
		}
	}

	for (auto& mesh : meshesToDelete)
	{
		meshesThatUseThisMaterial.erase(std::remove(meshesThatUseThisMaterial.begin(), meshesThatUseThisMaterial.end(), mesh), meshesThatUseThisMaterial.end());
	}
}

// Registers a new mesh that uses this material
void Material::RegisterNewMesh(std::string meshName)
{
	meshesThatUseThisMaterial.push_back(meshName);
}