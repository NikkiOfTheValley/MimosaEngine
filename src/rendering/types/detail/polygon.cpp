#include "../polygon.h"
#include "rendering/texture_manager.h"

Polygon2D::Polygon2D() {}

Polygon2D::Polygon2D(std::vector<vert2d> verts)
{
	this->verts = verts;

	// Generate the vertex arrays and buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
}

void Polygon2D::InitBuffers()
{
	// Generate the vertex arrays and buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
}

void Polygon2D::UpdateMesh()
{
	// If there's no mesh data, don't do anything
	if (verts.size() == 0)
		return;

	glBindVertexArray(vao);

	// Using GL_DYNAMIC_DRAW because it tells the GPU to put this in memory that allows faster writes (and thus less lag when updating the mesh)
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert2d) * verts.size(), &verts[0], GL_DYNAMIC_DRAW);

	// Only do this when first initializing the Mesh, as the vertex attributes don't need to change when updating the mesh
	if (firstInit) {
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		firstInit = false;
	}

	// Unbind everything so it can't be accidentally modified
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Polygon2D::Draw()
{
	// If there's no vert data, don't do anything
	if (verts.size() == 0)
		return;

	if (texture != nullptr)
		texture->Bind(0);

	shader->Bind();

	shader->SetVec2("pos", position);
	shader->SetVec2("screenDim", *Renderer::getInstance().screenDim);

	if (texture)
	{
		shader->SetVec2("texSize", (float)texture->w, (float)texture->h);

		if (texture->IsAtlasTexture())
		{
			//logger.log(texture->GetLocation().first);
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

	shader->SetBool("uvIsNDC", uvIsNDC);
	shader->SetBool("vertIsNDC", vertIsNDC);
	shader->SetBool("posIsNDC", posIsNDC);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verts.size());
}

void Polygon2D::Dealloc()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	verts.clear();
}