#include "font.h"
#include <rendering/types/image.h>
#include <rendering/texture_manager.h>
#include <rendering/renderer.h>

using namespace math;

namespace ui
{
	Font::Font(FT_Library* ft, std::string path, unsigned int height, bool useRGB)
	{
		logger.log("Loading font " + path);

		this->path = path;

		FT_Face face;

		if (FT_New_Face(*ft, path.c_str(), 0, &face))
			logger.fatal("Failed to create FT_Face for font " + path);

		LoadFontFromFace(face, height, useRGB);

		FT_Done_Face(face);
	}

	Font::Font(FT_Library* ft, const unsigned char* data, size_t dataLen, unsigned int height, bool useRGB)
	{
		logger.log("Loading embedded font");

		this->path = "embedded_font:" + std::to_string(rand());

		FT_Face face;

		if (FT_New_Memory_Face(*ft, data, (FT_Long)dataLen, 0, &face))
			logger.fatal("Failed to create FT_Face for font " + path);

		LoadFontFromFace(face, height, useRGB);

		FT_Done_Face(face);
	}

	std::string Font::GetPath() const
	{
		return path;
	}

	void Font::LoadFontFromFace(FT_Face& face, unsigned int height, bool useRGB)
	{
		math::vec2 screenSize = *Renderer::getInstance().screenDim;

		int physicalScreenWidth = 0;
		int physicalScreenHeight = 0;

		glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &physicalScreenWidth, &physicalScreenHeight);

		float xScale, yScale;

		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xScale, &yScale);

		float dpiX = 170;
		float dpiY = 96;

		if (physicalScreenHeight == NULL || physicalScreenWidth == NULL)
		{
			logger.warn("Could not get monitor DPI, assuming 96 DPI");

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
			logger.err("Failed to set character size for font " + path);

		for (unsigned char c = 32; c < 128; c++)
		{
			FT_Int32 loadFlags = FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT;

			if (useRGB)
				loadFlags = FT_LOAD_RENDER | FT_LOAD_TARGET_LCD;

			// Load the character
			if (FT_Load_Char(face, c, loadFlags))
			{
				logger.err((std::stringstream() << "Failed to load glyph for character `" << c).str() + "` in font " + path);
				continue;
			}

			if (face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY && face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_LCD)
			{
				logger.err((std::stringstream() << "Character `" << c).str() + "` in font " + path + " is in an invalid pixel mode!");
				continue;
			}

			vec2 bitmapDimensions = { face->glyph->bitmap.width, face->glyph->bitmap.rows };

			// Convert the FreeType 8bpp bitmap into an Image
			Image* img = new Image(face->glyph->bitmap.buffer, (int)bitmapDimensions.x, (int)bitmapDimensions.y, useRGB, true, true);

			// Add the Image to the texture atlas
			TextureManager::getInstance().AddImage(img, path + std::to_string(c));

			// Add the glyph info into the character map
			if (!useRGB)
			{
				glyphInfo_t glyphInfo =
				{
					TextureManager::getInstance().GetTextureLocation(path + std::to_string(c)),
					math::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					math::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					(unsigned int)(face->glyph->advance.x >> 6),
				};
				characters.insert(std::pair<char, glyphInfo_t>(c, glyphInfo));
			}
			else
			{
				glyphInfo_t glyphInfo =
				{
					TextureManager::getInstance().GetTextureLocation(path + std::to_string(c)),
					math::vec2(face->glyph->bitmap.width / 3, face->glyph->bitmap.rows),
					math::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					(unsigned int)(face->glyph->advance.x >> 6),
				};
				characters.insert(std::pair<char, glyphInfo_t>(c, glyphInfo));
			}

		}
	}
}