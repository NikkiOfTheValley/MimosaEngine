#include "../texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

// Use this texture
void Texture::Bind(int id)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE0 + id);
}

bool Texture::IsRGBA()
{
	return isRGBA;
}