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
	text = new Text(str, vec2((center.data[0] + dim.data[0] / 2) - (4 * textScale * str.length() / screenDim->data[0]), center.data[1] + (4 / screenDim->data[1])), textScale);

	this->dim = dim;

	// Construct background polygon
	Polygon2D* poly = new Polygon2D();
	poly->InitBuffers();
	poly->verts = {
		vert2d{ dim.data[0], dim.data[1], 1.f, 1.f },
		vert2d{ 0.f,         dim.data[1], 0.f, 1.f },
		vert2d{ 0.f,         0.f,         0.f, 0.f },
		vert2d{ dim.data[0], 0.f,         1.f, 0.f },
		vert2d{ dim.data[0], dim.data[1], 1.f, 1.f },
		vert2d{ 0.f,         0.f,         0.f, 0.f } };
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

	text = new Text(str, vec2((center.data[0] + dim.data[0] / 2) - (4 * textScale * str.length() / screenDim->data[0]), center.data[1] + (4 / screenDim->data[1])), textScale);

	// Construct background polygon
	Polygon2D* poly = new Polygon2D();
	poly->InitBuffers();
	poly->verts = {
		vert2d{ dim.data[0], dim.data[1], 1.f, 1.f },
		vert2d{ 0.f,         dim.data[1], 0.f, 1.f },
		vert2d{ 0.f,         0.f,         0.f, 0.f },
		vert2d{ dim.data[0], 0.f,         1.f, 0.f },
		vert2d{ dim.data[0], dim.data[1], 1.f, 1.f },
		vert2d{ 0.f,         0.f,         0.f, 0.f } };
	poly->position = pos;
	poly->shader = ResourceManager::getInstance().GetShader("flatShader");
	poly->texture = ResourceManager::getInstance().GetTexture("buttonBackground");
	poly->uvIsNDC = true;
	poly->vertIsNDC = true;
	poly->posIsNDC = true;

	// Check if the mouse is within the bounds of the button
	if (mousePos.data[0] / screenDim->data[0] > pos.data[0] && mousePos.data[0] / screenDim->data[0] < pos.data[0] + dim.data[0] &&
		mousePos.data[1] / screenDim->data[1] > pos.data[1] && mousePos.data[1] / screenDim->data[1] < pos.data[1] + dim.data[1])
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