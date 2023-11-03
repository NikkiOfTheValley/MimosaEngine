#include "text_box.h"

TextBox::TextBox(std::string defaultStr, vec2 pos, vec2 dim, int textScale)
{
	this->pos = pos;
	this->str = defaultStr;
	this->textScale = textScale;

	// Make sure the text on the text box is center-aligned

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
	poly->texture = ResourceManager::getInstance().GetTexture("textBox");
	poly->uvIsNDC = true;
	poly->vertIsNDC = true;
	poly->posIsNDC = true;
	poly->UpdateMesh();

	backgroundImage = poly;
}

TextBox::~TextBox()
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

void TextBox::Draw()
{
	text->Draw();
	backgroundImage->Draw();
}

void TextBox::Update()
{
	// If this text box has been selected, then the text on the text box should match the input string
	if (selected)
		str = textInput;

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

	ResourceManager* resourceManager = &ResourceManager::getInstance();

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
	poly->shader = resourceManager->GetShader("flatShader");
	selected ? poly->texture = resourceManager->GetTexture("textBoxHighlighted") :
			   poly->texture = resourceManager->GetTexture("textBox");
	poly->uvIsNDC = true;
	poly->vertIsNDC = true;
	poly->posIsNDC = true;

	// Check if the mouse is within the bounds of the text box
	if (mousePos.x / screenDim->x > pos.x && mousePos.x / screenDim->x < pos.x + dim.x &&
		mousePos.y / screenDim->y > pos.y && mousePos.y / screenDim->y < pos.y + dim.y)
	{
		// Check if the left mouse button has been pressed
		if (lmbPressed)
		{
			// Highlight the text box
			poly->texture = resourceManager->GetTexture("textBoxHighlighted");
			selected = true;
			useTextInput = true;
		}
	}
	else
	{
		// If the left mouse button has been pressed outside of the text box, deselect it
		if (lmbPressed)
		{
			poly->texture = resourceManager->GetTexture("textBox");
			selected = false;
			useTextInput = false;
		}
	}

	poly->UpdateMesh();
	backgroundImage = poly;
}