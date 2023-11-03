#include "button.h"

Button::Button(std::string str, vec2 pos, vec2 dim, int textScale, std::function<void()> func)
{
	this->func = func;
	this->pos = pos;
	this->str = str;
	this->textScale = textScale;

	// Make sure the text on the button is center-aligned

	vec2 center = (pos * 2) + (dim / 2);

	vec2* screenDim = Renderer::getInstance().screenDim;
	text = new Text(str, vec2((center.x + dim.x / 2) - (4 * textScale * str.length() / screenDim->x), center.y + (4 / screenDim->y)), textScale);

	this->dim = dim;

	// Construct background polygon
	Polygon2D* poly = new Polygon2D();
	poly->InitBuffers();
	poly->verts = {
		vert2d{ dim.x, dim.y, 1.f, 1.f },
		vert2d{ 0.f,   dim.y, 0.f, 1.f },
		vert2d{ 0.f,   0.f,   0.f, 0.f },
		vert2d{ dim.x, 0.f,   1.f, 0.f },
		vert2d{ dim.x, dim.y, 1.f, 1.f },
		vert2d{ 0.f,   0.f,   0.f, 0.f } };
	poly->position = pos;
	poly->shader = ResourceManager::getInstance().GetShader("flatShader");
	poly->texture = ResourceManager::getInstance().GetTexture("buttonBackground");
	poly->uvIsNDC = true;
	poly->vertIsNDC = true;
	poly->posIsNDC = true;
	poly->UpdateMesh();

	backgroundImage = poly;

}

Button::~Button()
{
	backgroundImage->Dealloc();
	delete backgroundImage;

	for (auto& p : text->letterPolygons)
	{
		p.Dealloc();
	}
	text->letterPolygons.clear();
	delete text;
}

void Button::Draw()
{
	text->Draw();
	backgroundImage->Draw();
}

void Button::Update()
{
	// Update visuals to match screen size

	backgroundImage->Dealloc();
	delete backgroundImage;

	for (auto& p : text->letterPolygons)
	{
		p.Dealloc();
	}
	text->letterPolygons.clear();
	
	delete text;
	
	vec2 center = (pos * 2) + (dim / 2);
	vec2* screenDim = Renderer::getInstance().screenDim;

	text = new Text(str, vec2((center.x + dim.x / 2) - (4 * textScale * str.length() / screenDim->x), center.y + (4 / screenDim->y)), textScale);

	// Construct background polygon
	Polygon2D* poly = new Polygon2D();
	poly->InitBuffers();
	poly->verts = {
		vert2d{ dim.x, dim.y, 1.f, 1.f },
		vert2d{ 0.f,   dim.y, 0.f, 1.f },
		vert2d{ 0.f,   0.f,   0.f, 0.f },
		vert2d{ dim.x, 0.f,   1.f, 0.f },
		vert2d{ dim.x, dim.y, 1.f, 1.f },
		vert2d{ 0.f,   0.f,   0.f, 0.f } };
	poly->position = pos;
	poly->shader = ResourceManager::getInstance().GetShader("flatShader");
	poly->texture = ResourceManager::getInstance().GetTexture("buttonBackground");
	poly->uvIsNDC = true;
	poly->vertIsNDC = true;
	poly->posIsNDC = true;

	// Check if the mouse is within the bounds of the button
	if (mousePos.x / screenDim->x > pos.x && mousePos.x / screenDim->x < pos.x + dim.x &&
		mousePos.y / screenDim->y > pos.y && mousePos.y / screenDim->y < pos.y + dim.y)
	{
		// Highlight the button
		poly->texture = ResourceManager::getInstance().GetTexture("buttonBackgroundHighlighted");

		// Check if the button has been pressed
		if (lmbPressed)
			func();
	}

	poly->UpdateMesh();
	backgroundImage = poly;
}