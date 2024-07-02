#include "text.h"
#include <rendering/texture_manager.h>
#include <ui/font_manager.h>

static int globalIDCounter = 0;

Text::Text(std::string text, std::shared_ptr<FontManager> fontManager, std::string fontName, vec2 pos, int scale, bool isStatic)
{
	this->scale = scale;
	this->isStatic = isStatic;
	this->pos = pos;
	this->text = text;
	this->fontName = fontName;
	this->polygonsStale = false;

	this->fontManager = fontManager;

	const Font* font = fontManager->GetFont(fontName);

	GenerateText(font);

}

void Text::Update()
{
	const Font* font = fontManager->GetFont(fontName);

	if (polygonsStale && !isStatic)
		GenerateText(font);
	else
	{
		vec2* screenDim = Renderer::getInstance().screenDim;

		// Start the text at the top-left of the first character
		vec2 currentPosition = pos * *screenDim;

		float firstGlyphHeight = font->characters.at(text[0]).size.y;

		size_t i = 0;
		for (Polygon2D& poly : letterPolygons)
		{
			glyphInfo_t glyph = font->characters.at(text[i]);

			poly.position = vec2(currentPosition.x, currentPosition.y - glyph.bearing.y + firstGlyphHeight);

			poly.UpdateMesh();

			currentPosition.x += glyph.advance;

			i++;
		}
	}
}

void Text::UpdateText(std::string newText)
{
	if (isStatic)
	{
		Logger::getInstance().err("Attempted to change the text of a static Text object!");
		return;
	}
	 
	text = newText;
	polygonsStale = true;
}

void Text::UpdatePosition(vec2 newPos, int newScale)
{
	if (isStatic)
	{
		Logger::getInstance().err("Attempted to change the position/scale of a static Text object!");
		return;
	}

	pos = newPos;
	scale = newScale;

	Update();
}

void Text::Draw()
{
	for (auto& p : letterPolygons)
	{
		p.Draw();
	}
}

void Text::DeallocPolygons()
{
	for (auto& p : letterPolygons)
	{
		p.Dealloc();
	}

	letterPolygons.clear();
}

void Text::GenerateText(const Font* font)
{
	// Delete the existing polygons
	DeallocPolygons();

	vec2* screenDim = Renderer::getInstance().screenDim;

	// Start the text at the top-left of the first character
	vec2 currentPosition = pos * *screenDim;

	float firstGlyphHeight = font->characters.at(text[0]).size.y;

	for (char chr : text)
	{
		glyphInfo_t glyph = font->characters.at(chr);

		Polygon2D poly = Polygon2D();
		poly.verts = {
			vert2d{ glyph.size.x, glyph.size.y, 1.f, 1.f },
			vert2d{ 0.f,		  glyph.size.y, 0.f, 1.f },
			vert2d{ 0.f,		  0.f,			0.f, 0.f },
			vert2d{ glyph.size.x, 0.f,			1.f, 0.f },
			vert2d{ glyph.size.x, glyph.size.y, 1.f, 1.f },
			vert2d{ 0.f,		  0.f,			0.f, 0.f }
		};
		
		poly.position = vec2(currentPosition.x, currentPosition.y - glyph.bearing.y + firstGlyphHeight);

		poly.shader = ResourceManager::getInstance().GetShader("flatShader");

		poly.texture = ResourceManager::getInstance().GetTexture(font->GetPath() + std::to_string(chr));
		poly.posIsNDC = false;
		poly.uvIsNDC = true;

		poly.InitBuffers();
		poly.UpdateMesh();

		letterPolygons.push_back(poly);

		currentPosition.x += glyph.advance;
	}
}