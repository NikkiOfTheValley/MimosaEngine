#pragma once
#include <math/vec.h>
#include "ui/types/text.h"
#include <resource/resource_manager.h>
#include <functional>
#include "button.h"

namespace ui
{
	class TextBox
	{
	public:
		std::string str;

	public:
		TextBox(std::string defaultStr, std::shared_ptr<FontManager> fontManager, std::string font, math::vec2 pos, math::vec2 dim, int textScale);
		~TextBox();

		void Draw();

		void Update();

	private:
		math::vec2 pos;
		math::vec2 dim;
		Text* text;
		int textScale;
		Polygon2D* backgroundImage;
		bool selected = false;
	};
}