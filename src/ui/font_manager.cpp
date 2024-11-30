#include "font_manager.h"
#include "rendering/texture_manager.h"

namespace ui
{
	FontManager::FontManager()
	{
		logger.log("Initializing text rendering");

		if (FT_Init_FreeType(&ft))
			logger.fatal("Failed to initialize FreeType!");
	}

	void FontManager::AddNewFont(std::string path, std::string name, unsigned int height, bool useRGB)
	{
		Font* font = new Font(&ft, path, height, useRGB);

		fontNameToIndex[name] = fonts.size();
		fonts.push_back(font);
	}

	void FontManager::AddNewFont(const unsigned char* data, size_t len, std::string name, unsigned int height, bool useRGB)
	{
		Font* font = new Font(&ft, data, len, height, useRGB);

		fontNameToIndex[name] = fonts.size();
		fonts.push_back(font);
	}

	glyphInfo_t FontManager::GetGlyph(char c, std::string fontName)
	{
		if (!fontNameToIndex.contains(fontName))
		{
			logger.err("Font " + fontName + " does not exist or has not been added");
			return {};
		}

		Font* font = fonts[fontNameToIndex[fontName]];

		return font->characters[c];
	}

	const Font* FontManager::GetFont(std::string fontName)
	{
		if (!fontNameToIndex.contains(fontName))
		{
			logger.err("Font " + fontName + " does not exist or has not been added");
			return {};
		}

		return fonts[fontNameToIndex[fontName]];
	}
}