#pragma once
#include <string>

// This is a representation of an RGB or RGBA image, which can be edited by code and loaded by the TextureManager

class Image
{
public:
	Image(std::string path, bool isRGBA);

	Image(uint32_t* data, int width, int height, bool isRGBA);

	// Converts a FreeType bitmap into a font image
	Image(uint8_t* data, int width, int height, bool useRGB, bool isResultRGBA, bool flipVertically = false);

	bool IsRGBA();

	// Blit a portion of this image onto `dest` at destX, destY
	void Blit(Image* dest, size_t srcX, size_t srcY, size_t srcWidth, size_t srcHeight, size_t destX, size_t destY);

	int w;
	int h;

	uint32_t* data;
private:

	bool isRGBA;

	std::string path;
};