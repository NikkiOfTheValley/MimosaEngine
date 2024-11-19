#pragma once
#include <math/vec.h>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ui
{
	struct glyphInfo_t {
		std::pair<math::vec2, math::vec2> atlasPosition; // Position of the glyph in the texture atlas
		math::vec2 size; // Size of the glyph
		math::vec2 bearing; // Offset from baseline to top-left of the glyph
		unsigned int advance; // Offset to advance to the next glyph position
	};

	class Font
	{
	public:
		Font(FT_Library* ft, std::string path, unsigned int height, bool useRGB);

		Font(FT_Library* ft, const unsigned char* data, size_t dataLen, unsigned int height, bool useRGB);

		std::string GetPath() const;

		std::map<char, glyphInfo_t> characters;

	private:
		std::string path;

		void LoadFontFromFace(FT_Face& face, unsigned int height, bool useRGB);
	};
}