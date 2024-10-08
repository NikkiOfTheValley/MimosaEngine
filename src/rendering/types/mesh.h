#pragma once
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math/vec.h>
#include "core/globals.h"
#include "../renderer.h"
#include "math/conversion.h"
#include "material.h"


struct vert
{
	float x;
	float y;
	float z;
	float u;
	float v;
	float normX;
	float normY;
	float normZ;
};

class Mesh
{
public:
	std::vector<vert> verts;

	math::vec3 position = math::vec3(0.f, 0.f, 0.f);
	math::vec3 rotation = math::vec3(0.f, 0.f, 0.f);

	Mesh() = default;

	Mesh(std::vector<vert> verts, Material* material);

	// Initailizes the VAO and VBO in the case that the default constructor was used
	void InitBuffers();

	// Updates the mesh on the GPU
	void UpdateMesh();

	void Draw(math::mat4x4f viewMatrix, math::mat4x4f projectionMatrix);

private:
	GLuint vao;
	GLuint vbo;

	bool firstInit = true;

	Material* material;
};