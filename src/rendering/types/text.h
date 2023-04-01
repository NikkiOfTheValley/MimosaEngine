#pragma once
#include "polygon.h"
#include "shader.h"
#include "../core/resource_manager.h"

class Text
{
public:
	std::vector<Polygon2D> letterPolygons;

	Text(std::string text, vec2 pos, int scale);

	void Update();

	void UpdateText(std::string newText);

	void Draw();

private:
	int scale;
	vec2 pos;
	std::string text;
};