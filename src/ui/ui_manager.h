#pragma once
#include "rendering/types/polygon.h"
#include "types/text.h"
#include "types/button.h"
#include "types/text_box.h"
#include "font_manager.h"

namespace ui
{
	struct UI_STATE
	{
		std::string name;

		std::vector<Polygon2D*> imagePolygons;
		std::vector<std::pair<Text*, std::string>> textElements;
		std::vector<std::pair<TextBox*, std::string>> textBoxElements;
		std::vector<Button*> buttons;
	};

	class UIManager
	{
	public:
		std::string curUIStateName;

	public:
		UIManager();

		// Creates a new image element in the current UI state
		void CreateImageElement(std::shared_ptr<Texture> tex, bool isFullscreen, vec2 pos = vec2(), vec2 dim = vec2());

		// Creates a new text element in the current UI state
		void CreateTextElement(std::string str, std::string name, vec2 pos, int scale, bool isStatic = false);

		// Creates a new button element in the current UI state that calls a given function when pressed
		void CreateButtonElement(std::string str, vec2 pos, vec2 dim, int textScale, std::function<void()> func);

		// Creates a new text box element in the current UI state
		void CreateTextBoxElement(std::string defaultStr, std::string name, vec2 pos, vec2 dim, int textScale);

		// Updates a text element in the current UI state
		void UpdateTextElement(std::string name, std::string str = "");

		std::string GetTextBoxContents(std::string name);

		// Creates a new UI
		void NewUI(std::string name);

		// Selects a previously created UI
		void SelectUI(std::string name);

		// Update the position of elements depending on aspect ratio
		void Update();

		void Draw();

	private:
		// All the different UIs that could be displayed or interacted with
		std::vector<UI_STATE> states;
		UI_STATE* curState = nullptr;

		std::shared_ptr<FontManager> fontManager;
	};
}