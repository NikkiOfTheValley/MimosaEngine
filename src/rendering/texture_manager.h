#pragma once
#include "types/texture.h"

// The TextureManager is a Singleton because more than one thing accesses it while also needing global state

class TextureManager
{
	// Should be thread-safe, magic statics (also known as "dynamic initialization and destruction with concurrency") were implemented in VS 2015
	static TextureManager& getInstance()
	{
		static TextureManager instance;
		return instance;
	}




	TextureManager(TextureManager const&) = delete;
	void operator=(TextureManager const&) = delete;

private:
	TextureManager() {};

	// An atlas of all used textures. Allows more than 16 textures to be in use at once
	Texture* textureAtlas;


};