#pragma once
#include "rendering/types/vec.h"
#include "rendering/types/text.h"
#include "rendering/resource_manager.h"
#include <functional>
#include "button.h"

class TextBox
{
public:
	std::string str;

public:
	TextBox(std::string defaultStr, vec2 pos, vec2 dim, int textScale);
	~TextBox();

	void Draw();

	void Update();

private:
	vec2 pos;
	vec2 dim;
	Text* text;
	int textScale;
	Polygon2D* backgroundImage;
	bool selected = false;
};