#include "../texture.h"
#include "../../texture_manager.h"

#include <math/vec.h>

using namespace math;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/*
Used to label textures created from `Image`s.
Since an Image doesn't have a path, we have to give the texture an ID instead
 */
static size_t globalIDCounter = 0;

Texture::Texture(std::string path, bool isRGBA, bool useNearestNeighbor)
{
	this->isRGBA = isRGBA;
	this->path = path;
	this->useNearestNeighbor = useNearestNeighbor;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set the texture wrapping/filtering options on the currently bound texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useNearestNeighbor ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, useNearestNeighbor ? GL_NEAREST : GL_LINEAR);
	
	
	// Load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		// Check if the texture has an alpha channel. If so, tell OpenGL that it does
		if (isRGBA)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		if (!isRGBA)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
		w = width;
		h = height;
	}
	else
	{
		Logger::getInstance().err("Failed to load texture " + path);
	}
	stbi_image_free(data);
}

Texture::Texture(Image* image, bool useNearestNeighbor)
{
	this->isRGBA = isRGBA;
	this->path = "tex:" + std::to_string(globalIDCounter);
	globalIDCounter++;

	this->useNearestNeighbor = useNearestNeighbor;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set the texture wrapping/filtering options on the currently bound texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useNearestNeighbor ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, useNearestNeighbor ? GL_NEAREST : GL_LINEAR);


	// Generate the texture
	if (image->data)
	{
		// Check if the image has an alpha channel. If so, tell OpenGL that it does
		if (image->IsRGBA())
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

		glGenerateMipmap(GL_TEXTURE_2D);
		w = image->w;
		h = image->h;
	}
	else
	{
		Logger::getInstance().err("Failed to generate texture " + path + " from image");
	}
}

Texture::Texture(std::pair<math::vec2, math::vec2> atlasLoc)
{
	w = (int)abs((atlasLoc.first.x - atlasLoc.second.x) * TextureManager::getInstance().GetAtlasDimensions().x);
	h = (int)abs((atlasLoc.first.y - atlasLoc.second.y) * TextureManager::getInstance().GetAtlasDimensions().y);

	this->location = atlasLoc;
	this->isAtlasTexture = true;
}

// Gets the location of this texture in the atlas, if the texture is an atlased texture.
std::pair<math::vec2, math::vec2> Texture::GetLocation()
{
	if (!this->isAtlasTexture)
	{
		Logger::getInstance().warn("Attempted to get the atlas location of a non-atlas texture! This will cause broken textures!");
		return { math::vec2(), math::vec2() };
	}

	return this->location;
}

// Use this texture
void Texture::Bind(int id)
{
	if (isAtlasTexture)
	{
		TextureManager::getInstance().textureAtlasTexture->Bind(id);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE0 + id);
	}
}

bool Texture::IsRGBA()
{
	return isRGBA;
}

bool Texture::IsAtlasTexture()
{
	return isAtlasTexture;
}