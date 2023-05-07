#include "../text.h"

Text::Text(std::string text, vec2 pos, int scale)
{
	this->scale = scale;
	this->pos = pos;
	this->text = text;

	// Iterate over each character in the string, and place a Polygon2D for each one, with a UV offset into the font atlas

	size_t i = 0;
	uint64_t c = 0;
	for (int chr : text)
	{
		c = static_cast<uint64_t>(chr) - 32;
		Polygon2D poly = Polygon2D();
		poly.verts = {
			vert2d{ 8.f * scale, 8.f * scale, 8.f + (c * 8), 8.f },
			vert2d{ 0.f,         8.f * scale, 0.f + (c * 8), 8.f },
			vert2d{ 0.f,         0.f,         0.f + (c * 8), 0.f },
			vert2d{ 8.f * scale, 0.f,         8.f + (c * 8), 0.f },
			vert2d{ 8.f * scale, 8.f * scale, 8.f + (c * 8), 8.f },
			vert2d{ 0.f,         0.f,         0.f + (c * 8), 0.f }
		};
		vec2* screenDim = Renderer::getInstance().screenDim;
		poly.position = vec2(pos.data[0] + ((float)(i * 8 * scale) / screenDim->data[0]), pos.data[1] * screenDim->data[1]);
		
		poly.shader = ResourceManager::getInstance().GetShader("flatShader");
		poly.texture = ResourceManager::getInstance().GetTexture("textTexture");
		poly.posIsNDC = true;
		
		poly.InitBuffers();
		poly.UpdateMesh();

		letterPolygons.push_back(poly);

		i++;
	}
}

void Text::Update()
{
	// Delete the existing Polygon2Ds

	for (auto& p : letterPolygons)
	{
		p.Dealloc();
	}

	letterPolygons.clear();

	// Iterate over each character in the current string, and place a Polygon2D for each one, with a UV offset into the font atlas

	size_t i = 0;
	uint64_t c = 0;
	for (int chr : text)
	{
		c = (uint64_t)chr - 32;
		Polygon2D poly = Polygon2D();
		poly.verts = {
			vert2d{ 8.f * scale, 8.f * scale, 8.f + (c * 8), 8.f },
			vert2d{ 0.f,         8.f * scale, 0.f + (c * 8), 8.f },
			vert2d{ 0.f,         0.f,         0.f + (c * 8), 0.f },
			vert2d{ 8.f * scale, 0.f,         8.f + (c * 8), 0.f },
			vert2d{ 8.f * scale, 8.f * scale, 8.f + (c * 8), 8.f },
			vert2d{ 0.f,         0.f,         0.f + (c * 8), 0.f }
		};
		vec2* screenDim = Renderer::getInstance().screenDim;
		poly.position = vec2(pos.data[0] + ((float)(i * 8 * scale) / screenDim->data[0]), pos.data[1] * screenDim->data[1]);

		poly.shader = ResourceManager::getInstance().GetShader("flatShader");
		poly.texture = ResourceManager::getInstance().GetTexture("textTexture");
		poly.posIsNDC = true;

		poly.InitBuffers();
		poly.UpdateMesh();

		letterPolygons.push_back(poly);

		i++;
	}
}

void Text::UpdateText(std::string newText)
{
	// Delete the existing Polygon2Ds
	for (auto& p : letterPolygons)
	{
		p.Dealloc();
	}

	letterPolygons.clear();

	// Iterate over each character in the string, and place a Polygon2D for each one, with a UV offset into the font atlas

	size_t i = 0;
	uint64_t c = 0;
	for (int chr : newText)
	{
		c = static_cast<uint64_t>(chr) - 32;
		Polygon2D poly = Polygon2D();
		poly.verts = {
			vert2d{ 8.f * scale, 8.f * scale, 8.f + (c * 8), 8.f },
			vert2d{ 0.f,         8.f * scale, 0.f + (c * 8), 8.f },
			vert2d{ 0.f,         0.f,         0.f + (c * 8), 0.f },
			vert2d{ 8.f * scale, 0.f,         8.f + (c * 8), 0.f },
			vert2d{ 8.f * scale, 8.f * scale, 8.f + (c * 8), 8.f },
			vert2d{ 0.f,         0.f,         0.f + (c * 8), 0.f }
		};
		vec2* screenDim = Renderer::getInstance().screenDim;
		poly.position = vec2(pos.data[0] + ((float)(i * 8 * scale) / screenDim->data[0]), pos.data[1] * screenDim->data[1]);

		poly.shader = ResourceManager::getInstance().GetShader("flatShader");
		poly.texture = ResourceManager::getInstance().GetTexture("textTexture");
		poly.posIsNDC = true;

		poly.InitBuffers();
		poly.UpdateMesh();

		letterPolygons.push_back(poly);

		i++;
	}
	text = newText;
}

void Text::Draw()
{
	for (auto& p : letterPolygons)
	{
		p.Draw();
	}
}
