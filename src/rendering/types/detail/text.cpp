#include "../text.h"
#include <rendering/texture_manager.h>

static int globalIDCounter = 0;

Text::Text(std::string text, vec2 pos, int scale, bool isStatic)
{
	this->scale = scale;
	this->isStatic = isStatic;
	this->pos = pos;
	this->text = text;
	this->polygonsStale = false;

	if (isStatic)
	{
		// Generate just a single polygon, and simply blit together the static text into an Image, then into the texture atlas

		TextureManager* textureManager = &TextureManager::getInstance();

		// The location of the text atlas in the texture atlas, in pixels
		vec2 textTextureUV = (textureManager->GetTextureLocation("textTexture").first) * textureManager->GetAtlasDimensions();

		// Create a new Image sized to the text

		uint32_t* data = new uint32_t[8 * text.size() * 8];

		std::memset(data, 0, 8 * text.size() * 8 * sizeof(uint32_t));

		Image* textAsImage = new Image(data, 8 * (int)text.size(), 8, true);

		// Now loop over every character and blit them from the texture atlas to the newly created Image

		for (size_t i = 0; i < text.size(); i++)
		{
			char c = text[i] - 32;

			textureManager->textureAtlas->Blit(textAsImage, (size_t)(textTextureUV.x) + (c * 8), (size_t)textTextureUV.y, 8, 8, i * 8, 0);
		}

		// Now add the text Image to the texture atlas

		std::string name = "text:" + std::to_string(globalIDCounter);
		globalIDCounter++;

		textureManager->AddImage(textAsImage, name, true, false);

		Polygon2D poly = Polygon2D();
		poly.verts = {
			vert2d{ 8.f * text.size() * scale, 8.f * scale, 8.f * text.size(), 8.f },
			vert2d{ 0.f,					   8.f * scale, 0.f * text.size(), 8.f },
			vert2d{ 0.f,					   0.f,         0.f * text.size(), 0.f },
			vert2d{ 8.f * text.size() * scale, 0.f,         8.f * text.size(), 0.f },
			vert2d{ 8.f * text.size() * scale, 8.f * scale, 8.f * text.size(), 8.f },
			vert2d{ 0.f,					   0.f,         0.f * text.size(), 0.f }
		};
		vec2* screenDim = Renderer::getInstance().screenDim;
		poly.position = vec2(pos.x + ((float)(8 * scale) / screenDim->x), pos.y * screenDim->y);

		poly.shader = ResourceManager::getInstance().GetShader("flatShader");
		poly.texture = ResourceManager::getInstance().GetTexture(name);
		poly.posIsNDC = true;

		poly.InitBuffers();
		poly.UpdateMesh();

		letterPolygons.push_back(poly);

	}
	else
	{
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
			poly.position = vec2(pos.x + ((float)(i * 8 * scale) / screenDim->x), pos.y * screenDim->y);

			poly.shader = ResourceManager::getInstance().GetShader("flatShader");
			poly.texture = ResourceManager::getInstance().GetTexture("textTexture");
			poly.posIsNDC = true;

			poly.InitBuffers();
			poly.UpdateMesh();

			letterPolygons.push_back(poly);

			i++;
		}
	}

}

void Text::Update()
{
	// Update the text if the polygons are stale and the text isn't static
	if (!isStatic && polygonsStale)
		UpdateText(this->text);
	else
	{
		// Iterate over each character in the current string, and update the polygon position for each one
		size_t i = 0;
		for (Polygon2D &poly : letterPolygons)
		{
			vec2* screenDim = Renderer::getInstance().screenDim;
			poly.position = vec2(pos.x + ((float)(i * 8 * scale) / screenDim->x), pos.y * screenDim->y);

			poly.UpdateMesh();

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
	

	// Delete the existing polygons
	DeallocPolygons();

	// Iterate over each character in the string, and place a polygon for each one, with a UV offset into the font atlas

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
		poly.position = vec2(pos.x + ((float)(i * 8 * scale) / screenDim->x), pos.y * screenDim->y);

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
