#pragma once
#include <string>

/*!
 * @brief This is a representation of an RGB or RGBA image, which can be edited by code and loaded by the TextureManager
 */
class Image
{
public:
	/*!
	 * @brief Loads an image from the filesystem
	 * @param path The path to the image
	 * @param isRGBA Is the image RGBA
	 */
	Image(std::string path, bool isRGBA);

	/*!
	 * @brief Loads an embedded image file
	 * @param data The embedded data
	 * @param len The length of the embedded data
	 * @param isRGBA Is the image RGBA
	 */
	Image(const unsigned char* data, size_t len, bool isRGBA);

	/*!
	 * @brief Loads an image directly
	 * @param data The image data
	 * @param width The image width
	 * @param height The image height
	 * @param isRGBA Is the image RGBA
	 */
	Image(uint32_t* data, int width, int height, bool isRGBA);

	/*!
	 * @brief Converts a FreeType bitmap into a font image
	 * @param data The bitmap to convert
	 * @param width The width of the bitmap
	 * @param height The height of the bitmap
	 * @param useRGB Use the RGB channels and interpret the bitmap as a subpixel bitmap
	 * @param isResultRGBA Should the image use the alpha channel
	 * @param flipVertically Flip the image vertically
	 */
	Image(uint8_t* data, int width, int height, bool useRGB, bool isResultRGBA, bool flipVertically = false);

	bool IsRGBA();

	/*!
	 * @brief Blit a portion of this image
	 * @param dest The destination to blit to
	 * @param srcX The source portion's X coordinate
	 * @param srcY The source portion's X coordinate
	 * @param srcWidth The source portion's width
	 * @param srcHeight The source portion's height
	 * @param destX The destination portion's X coordinate
	 * @param destY The destination portion's Y coordinate
	 */
	void Blit(Image* dest, size_t srcX, size_t srcY, size_t srcWidth, size_t srcHeight, size_t destX, size_t destY);

	int w;
	int h;

	uint32_t* data;
private:

	bool isRGBA;

	std::string path;
};