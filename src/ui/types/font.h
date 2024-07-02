#pragma once
#include "rendering/types/vec.h"
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

struct glyphInfo_t {
	std::pair<vec2, vec2> atlasPosition; // Position of the glyph in the texture atlas
	vec2 size; // Size of the glyph
	vec2 bearing; // Offset from baseline to top-left of the glyph
	unsigned int advance; // Offset to advance to the next glyph position
};

class Font
{
public:
	Font(FT_Library* ft, std::string path, unsigned int height);

	std::string GetPath() const;

	std::map<char, glyphInfo_t> characters;

private:
	std::string path;
};