#pragma once
#include "rendering/types/polygon.h"
#include "rendering/types/shader.h"
#include <resource/resource_manager.h>
#include <memory>

namespace ui
{
	class FontManager;
	class Font;

	class Text
	{
	public:
		Text(std::string text, std::shared_ptr<FontManager> fontManager, std::string fontName, math::vec2 pos, int scale, bool isStatic = false);

		void Update();

		void UpdateText(std::string newText);

		void UpdatePosition(math::vec2 newPos, int newScale);

		void Draw();

		void DeallocPolygons();

	private:
		math::vec2 pos;
		int scale;
		bool isStatic;
		std::string text;
		std::string fontName;

		std::vector<Polygon2D> letterPolygons;
		bool polygonsStale;

		std::shared_ptr<FontManager> fontManager;

		void GenerateText(const Font* font);
	};
}