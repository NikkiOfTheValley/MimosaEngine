#include "text_box.h"
#include <rendering/texture_manager.h>

TextBox::TextBox(std::string defaultStr, std::shared_ptr<FontManager> fontManager, std::string font, vec2 pos, vec2 dim, int textScale)
{
	this->pos = pos;
	this->str = defaultStr;
	this->textScale = textScale;

	// Make sure the text on the text box is center-aligned

	vec2 center = (pos * 2) + (dim / 2);
	vec2* screenDim = Renderer::getInstance().screenDim;

	text = new Text(str, fontManager, font, vec2((center.x + dim.x / 2) - (4 * textScale * str.length() / screenDim->x), center.y + (4 / screenDim->y)), textScale);

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

	text->DeallocPolygons();
	delete text;

	// Make sure to clear currentlySelectedText and useTextInput if this TextBox is selected, so the user can't corrupt memory by typing in a deleted TextBox
	if (selected)
	{
		useTextInput = false;
		currentlySelectedText = nullptr;
	}
}

void TextBox::Draw()
{
	text->Draw();
	backgroundImage->Draw();
}

void TextBox::Update()
{
	backgroundImage->Dealloc();
	delete backgroundImage;

	//text->DeallocPolygons();
	//delete text;

	vec2 center = (pos * 2) + (dim / 2);
	vec2* screenDim = Renderer::getInstance().screenDim;

	text->UpdatePosition(vec2((center.x + dim.x / 2) - (4 * textScale * str.length() / screenDim->x), center.y + (4 / screenDim->y)), textScale);
	text->UpdateText(str);

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

			currentlySelectedText = &str;
		}
	}
	else
	{
		// If the left mouse button has been pressed outside of the text box, deselect it
		if (lmbPressed)
		{
			poly->texture = resourceManager->GetTexture("textBox");
			selected = false;

			// Check if another TextBox has been selected, and if so, don't clear useTextInput or
			// currentlySelectedText, as they have been set by the other TextBox
			if (currentlySelectedText == &str)
			{
				useTextInput = false;
				currentlySelectedText = nullptr;
			}
		}
	}

	poly->UpdateMesh();
	backgroundImage = poly;
}