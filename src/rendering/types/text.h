#pragma once
#include "polygon.h"
#include "shader.h"
#include "../resource_manager.h"

class Text
{
public:
	Text(std::string text, vec2 pos, int scale, bool isStatic = false);

	void Update();

	void UpdateText(std::string newText);

	void UpdatePosition(vec2 newPos, int newScale);

	void Draw();

	void DeallocPolygons();

private:
	vec2 pos;
	int scale;
	bool isStatic;
	std::string text;

	std::vector<Polygon2D> letterPolygons;
	bool polygonsStale;

	void GenerateText(std::string text, vec2 pos, int scale);
	void GenerateTextStatic(std::string text, vec2 pos, int scale);
};