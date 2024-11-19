#include <stb_image.h>
#include <glad/glad.h>
#include "core/log.h"
#include "../image.h"
#include <math/vec2.h>

Image::Image(std::string path, bool isRGBA)
{
	this->isRGBA = isRGBA;
	this->path = path;

	// Load and generate the texture
	int nrChannels;
	data = (uint32_t*)stbi_load(path.c_str(), &w, &h, &nrChannels, 0);

	if (!data)
		Logger::getInstance().err("Failed to load texture " + path);
}

Image::Image(uint32_t* data, int width, int height, bool isRGBA)
{
	this->data = data;
	this->path = "img:" + std::to_string(rand());
	w = width;
	h = height;
	this->isRGBA = isRGBA;
}

Image::Image(uint8_t* data, int width, int height, bool useRGB, bool isResultRGBA, bool flipVertically)
{
	this->path = "img:" + std::to_string(rand());
	w = width;
	h = height;
	isRGBA = isResultRGBA;
	
	if ((width <= 0 && height > 0) || (height <= 0 && width > 0))
		Logger::getInstance().fatal("Image has invalid dimensions");

	if (!useRGB)
	{
		this->data = new uint32_t[width * height];
		memset(this->data, 0, (size_t)width * (size_t)height * sizeof(uint32_t));

		for (size_t x = 0; x < width; x++)
		{
			for (size_t y = 0; y < height; y++)
			{
				size_t scanlineIndex = y * width;

				if (flipVertically)
					scanlineIndex = (height - y - 1) * width;

				this->data[x + (y * width)] = ((uint32_t)0xFF) << 0;
				this->data[x + (y * width)] |= ((uint32_t)0xFF) << 8;
				this->data[x + (y * width)] |= ((uint32_t)0xFF) << 16;

				if (isResultRGBA)
					this->data[x + (y * width)] |= ((uint32_t)data[x + scanlineIndex]) << 24;

			}
		}
	}
	else
	{
		this->data = new uint32_t[(width / 3) * height];
		memset(this->data, 0, (size_t)(width / 3) * (size_t)height * sizeof(uint32_t));

		for (size_t x = 0; x < width; x += 3)
		{
			for (size_t y = 0; y < height; y++)
			{
				uint32_t value = 0;

				value |= (uint32_t)data[x + (y * width) + 0] << 0;
				value |= (uint32_t)data[x + (y * width) + 1] << 8;
				value |= (uint32_t)data[x + (y * width) + 2] << 16;
				
				if (isResultRGBA)
					value |= (uint32_t)0xFF << 24;

				this->data[x + (y * (width / 3))] = value;
			}
		}
	}

}

bool Image::IsRGBA() { return isRGBA; }

// Blit a portion of this image onto `dest` at destX, destY (Both images have to be RGBA!)
void Image::Blit(Image* dest, size_t srcX, size_t srcY, size_t srcWidth, size_t srcHeight, size_t destX, size_t destY)
{
	if (!isRGBA || !dest->IsRGBA())
	{
		Logger::getInstance().err("Failed to blit Image " + path + " to Image " + dest->path + " as one or both Image(s) aren't RGBA");
		return;
	}

	if (srcX + srcWidth > w || srcY + srcHeight > h || destX + srcWidth > dest->w || destY + srcHeight > dest->h)
	{
		Logger::getInstance().err("Failed to blit Image " + path + " to Image " + dest->path + " as the blitted portion is outside of one or both of the Image(s)");
		return;
	}

	for (size_t y = 0; y < srcHeight; y++)
	{
		std::memcpy(&dest->data[((destY + y) * dest->w) + destX], &data[((srcY + y) * w) + srcX], srcWidth * sizeof(uint32_t));

		//std::memset(&dest->data[((destY + y)*dest->w) + destX], 0xFF, srcWidth * sizeof(uint32_t));
	}
		

}