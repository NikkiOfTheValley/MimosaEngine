#pragma once
#include <math/vec.h>
#include "ui/types/text.h"
#include "rendering/resource_manager.h"
#include <functional>

namespace ui
{
	class Button
	{
	public:
		Button(std::string str, std::shared_ptr<FontManager> fontManager, std::string font, math::vec2 pos, math::vec2 dim, int textScale, std::function<void()> func);
		~Button();

		void Draw();

		void Update();

	private:
		math::vec2 pos;
		math::vec2 dim;
		std::function<void()> func;
		Text* text;
		std::string str;
		int textScale;
		Polygon2D* backgroundImage;
	};
}