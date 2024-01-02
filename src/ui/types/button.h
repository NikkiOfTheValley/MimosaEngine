#pragma once
#include "../rendering/types/vec.h"
#include "../rendering/types/text.h"
#include "../rendering/resource_manager.h"
#include <functional>

class Button
{
public:
	Button(std::string str, vec2 pos, vec2 dim, int textScale, std::function<void()> func);
	~Button();

	void Draw();

	void Update();

private:
	vec2 pos;
	vec2 dim;
	std::function<void()> func;
	Text* text;
	std::string str;
	int textScale;
	Polygon2D* backgroundImage;
};