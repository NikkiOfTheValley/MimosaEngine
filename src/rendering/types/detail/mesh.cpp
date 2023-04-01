#include "../mesh.h"

Mesh::Mesh(std::vector<vert> verts, Material* material)
{
	this->verts = verts;
	this->material = material;

	// Generate the vertex arrays and buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
}

void Mesh::InitBuffers()
{
	// Generate the vertex arrays and buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
}

void Mesh::UpdateMesh()
{
	// If there's no mesh data, don't do anything
	if (verts.size() == 0)
		return;

	glBindVertexArray(vao);

	// Using GL_DYNAMIC_DRAW because it tells the GPU to put this in memory that allows faster writes (and thus less lag when updating the mesh)
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert) * verts.size(), &verts[0], GL_DYNAMIC_DRAW);

	// Only do this when first initializing the Mesh, as the vertex attributes don't need to change when updating the mesh
	if (firstInit) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
		firstInit = false;
	}

	// Unbind everything so it can't be accidentally modified
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Draw(mat4x4f viewMatrix, mat4x4f projectionMatrix)
{
	// If there's no mesh data, don't do anything
	if (verts.size() == 0)
		return;

	mat4x4f modelMatrix;

	modelMatrix.translate(position);
	modelMatrix.rotate(rotation);

	material->Bind(viewMatrix, projectionMatrix, modelMatrix);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verts.size());
}