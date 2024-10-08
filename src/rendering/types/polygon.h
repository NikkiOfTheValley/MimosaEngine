#pragma once
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math/vec.h>
#include "shader.h"
#include "core/globals.h"
#include "../renderer.h"
#include "math/conversion.h"
#include "texture.h"

struct vert2d
{
	float x;
	float y;
	float u;
	float v;
};

class Polygon2D
{
public:
	std::vector<vert2d> verts;

	math::vec2 position = math::vec2(0.f, 0.f);

	Shader* shader;
	std::shared_ptr<Texture> texture;

	// If the UV coordinate is in normalized device coordinates, this must be true
	bool uvIsNDC = false;

	// If the vertex position coordinate is in normalized device coordinates, this must be true
	bool vertIsNDC = false;

	// If the position offset coordinate is in normalized device coordinates, this must be true
	bool posIsNDC = false;

	Polygon2D();

	Polygon2D(std::vector<vert2d> verts);

	// Initailizes the VAO and VBO in the case that the default constructor was used
	void InitBuffers();

	// Updates the mesh on the GPU
	void UpdateMesh();

	void Draw();

	void Dealloc();

private:
	GLuint vao;
	GLuint vbo;

	bool firstInit = true;
};