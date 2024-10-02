#include "ui_manager.h"

namespace ui
{
	UIManager::UIManager()
	{
		fontManager = std::make_shared<FontManager>();

		fontManager->AddNewFont("assets/engine/font/FreeMono.ttf", "defaultFont", 96);
	}

	// Creates a new image element in the current UI state
	void UIManager::CreateImageElement(std::shared_ptr<Texture> tex, bool isFullscreen, vec2 pos, vec2 dim)
	{
		if (!curState)
		{
			Logger::getInstance().err("No UIs exist or an invalid UI was selected.");
			return;
		}

		Polygon2D* poly = new Polygon2D();
		poly->InitBuffers();
		if (isFullscreen)
		{
			poly->verts = {
				vert2d{ 1.f, 1.f, 1.f, 1.f },
				vert2d{ 0.f, 1.f, 0.f, 1.f },
				vert2d{ 0.f, 0.f, 0.f, 0.f },
				vert2d{ 1.f, 0.f, 1.f, 0.f },
				vert2d{ 1.f, 1.f, 1.f, 1.f },
				vert2d{ 0.f, 0.f, 0.f, 0.f } };
		}
		else
		{
			poly->verts = {
				vert2d{ dim.x, dim.y, 1.f, 1.f },
				vert2d{ 0.f,   dim.y, 0.f, 1.f },
				vert2d{ 0.f,   0.f,   0.f, 0.f },
				vert2d{ dim.x, 0.f,   1.f, 0.f },
				vert2d{ dim.x, dim.y, 1.f, 1.f },
				vert2d{ 0.f,   0.f,   0.f, 0.f } };
		}
		poly->position = pos;
		poly->shader = ResourceManager::getInstance().GetShader("flatShader");
		poly->texture = tex;
		poly->uvIsNDC = true;
		poly->vertIsNDC = true;
		poly->posIsNDC = true;
		poly->UpdateMesh();

		curState->imagePolygons.push_back(poly);
	}

	// Creates a new text element in the current UI state
	void UIManager::CreateTextElement(std::string str, std::string name, vec2 pos, int scale, bool isStatic)
	{
		if (!curState)
		{
			Logger::getInstance().err("No UIs exist or an invalid UI was selected.");
			return;
		}

		Text* text = new Text(str, fontManager, "defaultFont", pos, scale, isStatic);

		std::pair<Text*, std::string> textElement = std::pair{ text, name };
		curState->textElements.push_back(textElement);
	}

	// Creates a new button element in the current UI state that calls a given function when pressed
	void UIManager::CreateButtonElement(std::string str, vec2 pos, vec2 dim, int textScale, std::function<void()> func)
	{
		if (!curState)
		{
			Logger::getInstance().err("No UIs exist or an invalid UI was selected.");
			return;
		}

		Button* button = new Button(str, fontManager, "defaultFont", pos, dim, textScale, func);
		curState->buttons.push_back(button);
	}

	// Creates a new text box element in the current UI state
	void UIManager::CreateTextBoxElement(std::string defaultStr, std::string name, vec2 pos, vec2 dim, int textScale)
	{
		if (!curState)
		{
			Logger::getInstance().err("No UIs exist or an invalid UI was selected.");
			return;
		}

		TextBox* textBox = new TextBox(defaultStr, fontManager, "defaultFont", pos, dim, textScale);
		curState->textBoxElements.push_back(std::pair(textBox, name));
	}

	// Updates a text element in the current UI state
	void UIManager::UpdateTextElement(std::string name, std::string str)
	{
		Logger* logger = &Logger::getInstance();

		if (!curState)
		{
			logger->err("No UIs exist or an invalid UI was selected.");
			return;
		}

		std::pair<Text*, std::string> textElement;
		textElement.first = nullptr;

		for (auto& t : curState->textElements)
		{
			if (t.second == name)
			{
				textElement = t;
				break;
			}
		}

		if (!textElement.first)
			logger->err("No text element named " + name + "  found!");
		else
		{
			if (str != "")
				textElement.first->UpdateText(str);
			else
				textElement.first->Update();
		}

	}

	std::string UIManager::GetTextBoxContents(std::string name)
	{
		Logger* logger = &Logger::getInstance();

		if (!curState)
		{
			logger->err("No UIs exist or an invalid UI was selected.");
			return "";
		}

		for (auto& elem : curState->textBoxElements)
		{
			if (elem.second == name)
				return elem.first->str;
		}
		logger->warn("No text box exists named \"" + name + "\"");
		return "";
	}

	// Creates a new UI
	void UIManager::NewUI(std::string name)
	{
		states.push_back(UI_STATE{ name, {}, {}, {} });
	}

	// Selects a previously created UI
	void UIManager::SelectUI(std::string name)
	{
		curState = nullptr;

		for (auto& state : states)
		{
			if (state.name == name)
			{
				curState = &state;
				break;
			}
		}
		if (!curState)
			Logger::getInstance().err("Selected invalid UI! This will cause broken behavior!");

		curUIStateName = name;
	}

	// Update the position of elements depending on aspect ratio
	void UIManager::Update()
	{
		if (!curState)
		{
			Logger::getInstance().err("No UIs exist or an invalid UI was selected.");
			return;
		}

		for (auto& p : curState->imagePolygons)
			p->UpdateMesh();

		for (auto& t : curState->textElements)
			t.first->Update();

		for (auto& b : curState->buttons)
			b->Update();

		for (auto& textBox : curState->textBoxElements)
			textBox.first->Update();
	}

	void UIManager::Draw()
	{
		if (!curState)
		{
			Logger::getInstance().err("No UIs exist or an invalid UI was selected.");
			return;
		}

		for (auto& b : curState->buttons)
			b->Draw();

		for (auto& textBox : curState->textBoxElements)
			textBox.first->Draw();

		for (auto& t : curState->textElements)
			t.first->Draw();

		for (auto& p : curState->imagePolygons)
			p->Draw();
	}
}