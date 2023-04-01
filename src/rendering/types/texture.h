#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../core/globals.h"

class Texture
{
	unsigned int texture;
	bool isRGBA = false;
public:
	Texture(std::string path, bool isRGBA, bool useNearestNeighbor);

	// Use this texture
	void Bind(int id);

	bool IsRGBA();

	int w;
	int h;
	std::string path;
	bool useNearestNeighbor;
};