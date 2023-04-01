#include "text_box.h"

TextBox::TextBox(std::string defaultStr, vec2 pos, vec2 dim, int textScale)
{
	this->pos = pos;
	this->str = defaultStr;
	this->textScale = textScale;

	// Make sure the text on the text box is center-aligned

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
	poly->shader = resourceManager->GetShader("flatShader");
	poly->texture = resourceManager->GetTexture("textBox");
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
	poly->shader = resourceManager->GetShader("flatShader");
	selected ? poly->texture = resourceManager->GetTexture("textBoxHighlighted") :
			   poly->texture = resourceManager->GetTexture("textBox");
	poly->uvIsNDC = true;
	poly->vertIsNDC = true;
	poly->posIsNDC = true;

	// Check if the mouse is within the bounds of the text box
	if (mousePos.data[0] / screenDim->data[0] > pos.data[0] && mousePos.data[0] / screenDim->data[0] < pos.data[0] + dim.data[0] &&
		mousePos.data[1] / screenDim->data[1] > pos.data[1] && mousePos.data[1] / screenDim->data[1] < pos.data[1] + dim.data[1])
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