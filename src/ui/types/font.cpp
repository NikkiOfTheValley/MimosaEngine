#include "font.h"
#include <rendering/types/image.h>
#include <rendering/texture_manager.h>
#include <rendering/renderer.h>

namespace ui
{
	Font::Font(FT_Library* ft, std::string path, unsigned int height)
	{
		Logger::getInstance().log("Loading font " + path);

		this->path = path;

		FT_Face face;

		if (FT_New_Face(*ft, path.c_str(), 0, &face))
			Logger::getInstance().fatal("Failed to create FT_Face for font " + path);

		vec2 screenSize = *Renderer::getInstance().screenDim;

		int physicalScreenWidth = 0;
		int physicalScreenHeight = 0;

		glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &physicalScreenWidth, &physicalScreenHeight);

		float xScale, yScale;

		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xScale, &yScale);

		float dpiX = 170;
		float dpiY = 96;

		if (physicalScreenHeight == NULL || physicalScreenWidth == NULL)
		{
			Logger::getInstance().warn("Could not get monitor DPI, assuming 96 DPI");

			dpiX /= xScale;
			dpiY /= yScale;
		}
		else
		{
			const float MM_TO_INCHES = 0.0393701f;

			dpiX = screenSize.x / (physicalScreenWidth * MM_TO_INCHES);
			dpiY = screenSize.y / (physicalScreenHeight * MM_TO_INCHES);

			dpiX /= xScale;
			dpiY /= yScale;
		}

		if (FT_Set_Char_Size(face, 0, height << 6, (int)dpiX, (int)dpiY))
			Logger::getInstance().err("Failed to set character size for font " + path);

		for (unsigned char c = 32; c < 128; c++)
		{
			// Load the character
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				Logger::getInstance().err(std::string("Failed to load glyph for character `" + c) + "` in font " + path);
				continue;
			}

			if (face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
			{
				Logger::getInstance().err(std::string("Character `" + c) + "` in font " + path + " is in the wrong pixel mode!");
				continue;
			}

			vec2 bitmapDimensions = { face->glyph->bitmap.width, face->glyph->bitmap.rows };

			// Convert the FreeType 8bpp bitmap into an Image
			Image* img = new Image(face->glyph->bitmap.buffer, (int)bitmapDimensions.x, (int)bitmapDimensions.y, true, true, true, true, true);

			// Add the Image to the texture atlas
			TextureManager::getInstance().AddImage(img, path + std::to_string(c), true, true);

			// Add the glyph info into the character map
			glyphInfo_t glyphInfo =
			{
				TextureManager::getInstance().GetTextureLocation(path + std::to_string(c)),
				vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				(unsigned int)(face->glyph->advance.x >> 6),
			};
			characters.insert(std::pair<char, glyphInfo_t>(c, glyphInfo));
		}

		FT_Done_Face(face);
	}

	std::string Font::GetPath() const
	{
		return path;
	}
}