#pragma once
#include "types/font.h"
#include "rendering/types/image.h"
#include <vector>
#include <unordered_map>

class FontManager
{
public:
	FontManager();

	void AddNewFont(std::string path, std::string name, unsigned int height);

	glyphInfo_t GetGlyph(char c, std::string fontName);

	const Font* GetFont(std::string fontName);

private:
	std::vector<Font*> fonts;
	std::unordered_map<std::string, size_t> fontNameToIndex;

	FT_Library ft;
};