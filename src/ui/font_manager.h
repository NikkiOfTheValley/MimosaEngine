#pragma once
#include "types/font.h"
#include "rendering/types/image.h"
#include <vector>
#include <unordered_map>

namespace ui
{
	class FontManager
	{
	public:
		FontManager();

		void AddNewFont(std::string path, std::string name, unsigned int height, bool useRGB);

		void AddNewFont(const unsigned char* data, size_t len, std::string name, unsigned int height, bool useRGB);

		glyphInfo_t GetGlyph(char c, std::string fontName);

		const Font* GetFont(std::string fontName);

	private:
		std::vector<Font*> fonts;
		std::unordered_map<std::string, size_t> fontNameToIndex;

		FT_Library ft;
	};
}