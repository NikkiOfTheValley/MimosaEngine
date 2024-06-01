#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/globals.h"
#include "image.h"
#include "vec.h"

class Texture
{
	unsigned int texture;
	bool isRGBA = false;
	
	// This is an ugly hack. But it allows `Texture` to represent both atlased textures and actual OpenGL texture objects,
	// which makes actually using this class much easier.
	// 
	// If anyone can think of a better way of doing this that still keeps this class easy to use (and doesn't require refactoring half the gfx code),
	// then please open a PR or open an issue explaining said better way.

	bool isAtlasTexture = false;
	std::pair<vec2, vec2> location = { vec2(-1, -1), vec2(-1, -1) };

public:
	Texture(std::string path, bool isRGBA, bool useNearestNeighbor);

	Texture(Image* image, bool useNearestNeighbor);

	Texture(std::pair<vec2, vec2> atlasLoc);

	// Gets the location of this texture in the atlas, if the texture is an atlased texture.
	std::pair<vec2, vec2> GetLocation();

	// Use this texture
	void Bind(int id);

	bool IsRGBA();

	bool IsAtlasTexture();

	int w;
	int h;
	std::string path;
	bool useNearestNeighbor;
};