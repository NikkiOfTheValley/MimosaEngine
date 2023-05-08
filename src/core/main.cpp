#include <chrono>
#include <thread>
#include "globals.h"
#include "../rendering/renderer.h"
#include "../rendering/types/shader.h"
#include "../rendering/types/mesh.h"
#include "input_handler.h"
#include <stb_image.h>
#include "../rendering/types/text.h"
#include "resource_manager.h"
#include "../ui/ui_manager.h"
#include "../rendering/types/camera.h"
#include "material_manager.h"

// - Global variables -

float fov = 45;
bool inUI = true;
vec2 mousePos = vec2();
bool lmbPressed = false;
bool useTextInput = false;
std::string textInput = "";

// - Local variables -

// deltaTime & FPS calculation variables

double deltaTime = 0.0f;
double frameTime = 0.0f;
double FPS = 0.0f;
double prevFPS = 0.0f;
uint64_t counter = 0; // Increments once every frame
double fixedTime = 0; // The amount of time since program startup in milliseconds

UIManager* uiManager;
MaterialManager materialManager;
ResourceManager* resourceManager;
Logger* logger;
bool enableFPSLimiter = true;
bool displayUI = true;
int setFPS = 60;

int main(int /*argc*/, char* /*argv[]*/)
{
	// -- Initialize --

	logger = &Logger::getInstance();

	logger->log(NAME_STR + " " + VERSION_STR);

	resourceManager = &ResourceManager::getInstance();
	uiManager = new UIManager();

	InputHandler inputHandler;

	Renderer* renderer = &Renderer::getInstance();

	if (!renderer->init(800, 600, inputHandler.mouse_callback, inputHandler.key_callback))
		return -1;


	// -- Load assets --

	stbi_set_flip_vertically_on_load(true);

	// We need flatShader and the font.png atlas to render text
	resourceManager->LoadShader("assets/flatShader.vert", "assets/flatShader.frag", "flatShader");
	resourceManager->LoadTexture("assets/font.png", true, true, "textTexture");

	// We need button.png and button-highlighted.png to render buttons
	resourceManager->LoadTexture("assets/button.png", true, true, "buttonBackground");
	resourceManager->LoadTexture("assets/button-highlighted.png", true, true, "buttonBackgroundHighlighted");

	// We need text-box.png and text-box-highlighted.png to render text boxes
	resourceManager->LoadTexture("assets/text-box.png", true, true, "textBox");
	resourceManager->LoadTexture("assets/text-box-highlighted.png", true, true, "textBoxHighlighted");

	resourceManager->LoadTexture("assets/example.png", true, true, "exampleImage");
	resourceManager->LoadTexture("assets/example2.png", true, true, "exampleImage2");
	resourceManager->LoadShader("assets/example.vert", "assets/example.frag", "exampleShader");

	// -- Create materials --

	materialManager.CreateMaterial("exampleMaterial", "exampleShader", "exampleImage2");

	// -- Create UIs --

	// First, create a new UI
	uiManager->NewUI("example");

	// Then, select it
	uiManager->SelectUI("example");

	// Then, start creating new elements on that new UI

	uiManager->CreateTextElement("Dynamic Example Text", "example", vec2{ 0.05f, 0.066f }, 4);

	uiManager->CreateTextElement("Static Example Text", "example_static", vec2{ 0.05f, 0.15f }, 4);

	uiManager->CreateImageElement(resourceManager->GetTexture("exampleImage"), false, vec2{ 0.25f, 0.1f }, vec2{ 0.25f, 0.25f });

	uiManager->CreateButtonElement("Example Button", vec2{ 0.41f, 0.36f }, vec2{ 0.38f, 0.08f }, 4,
		[&]() {
			uiManager->UpdateTextElement("example", "Dynamic Example Text 2");
		});

	uiManager->CreateTextBoxElement("Example Text Box", "text_box", vec2{ 0.355f, 0.5f }, vec2{ 0.28f, 0.06f }, 4);

	uiManager->CreateButtonElement("Example Button 2", vec2{ 0.355f, 0.7f }, vec2{ 0.38f, 0.08f }, 4,
		[&]() {
			std::string text = uiManager->GetTextBoxContents("text_box");

			logger->log(text);
		});

	// We have to select a UI before entering the render loop
	uiManager->SelectUI("example");

	const int numTests = 10;
	const int spacing = 100;

	for (int i = 0; i < numTests; i++)
	{
		uiManager->NewUI("test_" + std::to_string(i));
		uiManager->SelectUI("test_" + std::to_string(i));

		int numElements = (i + 1) * spacing;

		for (int k = 0; k < numElements; k++)
			uiManager->CreateTextElement("AAAAAAAAAAAA", "elem_" + std::to_string((i * spacing) + k), vec2{ 0.5f, 0.5f }, 4);
	}

	int curTest = 0;

	// We have to select a UI before entering the render loop
	uiManager->SelectUI("example");

	inUI = true;

	// -- Create meshes --

	renderer->CreateNewMesh(
		"assets/example.obj",
		"exampleMesh",
		materialManager.GetMaterial("exampleMaterial"),
		vec3{ 4.f, 0.f, 2.f });

	// We have to update the mesh before drawing it, otherwise the mesh
	// data is never sent to the GPU
	renderer->UpdateMesh("exampleMesh");
	
	// -- Create camera --

	Camera cam;
	cam.fov = 90.f;
	cam.pos = vec3{ 0, 0, 0 };
	
	bool lastStateOfF2 = false;
	bool lastStateOfF3 = false;

	// -- Render loop --

	while (!glfwWindowShouldClose(renderer->window))
	{
		auto startOfFrame = std::chrono::steady_clock::now();

		// -- Get input --

		inputHandler.processInput(renderer->window);

		// Reload assets if necessary

		if (inputHandler.reloadAssets)
		{
			logger->log("Reloading assets");
			resourceManager->ReloadShaders();
			resourceManager->ReloadTextures();
			materialManager.ReloadMaterials();
			uiManager->UpdateTextElement("example");
			inputHandler.reloadAssets = false;
		}

		// -- Draw --

		renderer->Draw(cam.GetViewMatrix(), cam.GetProjectionMatrix());

		if (displayUI)
		{
			// We have to clear the depth buffer or the UI might end up drawing under the geometry
			glClear(GL_DEPTH_BUFFER_BIT);
			uiManager->Draw();
		}

		glfwSwapBuffers(renderer->window);


		// -- Update --

		if (displayUI)
			uiManager->Update();

		if (lastStateOfF2 == false && glfwGetKey(renderer->window, GLFW_KEY_F2) == GLFW_PRESS)
			enableFPSLimiter = !enableFPSLimiter;
		lastStateOfF2 = glfwGetKey(renderer->window, GLFW_KEY_F2) == GLFW_PRESS;

		if (lastStateOfF3 == false && glfwGetKey(renderer->window, GLFW_KEY_F3) == GLFW_PRESS)
		{
			uiManager->SelectUI("test_" + std::to_string(curTest));
			curTest++;

			if (curTest == numTests)
				curTest = 0;
		}
		lastStateOfF3 = glfwGetKey(renderer->window, GLFW_KEY_F3) == GLFW_PRESS;

		if (glfwGetKey(renderer->window, GLFW_KEY_F4) == GLFW_PRESS)
			displayUI = !displayUI;

		cam.Update();

		renderer->GetMesh("exampleMesh")->rotation.data[1] += 0.5f * (float)deltaTime;

		glfwPollEvents();

		// deltaTime & FPS calculations
		// Don't ask me how this works. I barely know myself.

		using namespace std::chrono;

		auto endOfFrame = steady_clock::now();
		deltaTime = ((double)duration_cast<microseconds>(endOfFrame - startOfFrame).count() * 0.000001);
		frameTime = deltaTime;
		if (enableFPSLimiter)
			deltaTime += (1.f / (float)setFPS) - frameTime;

		auto nextFrame = duration_cast<steady_clock::duration>(duration<double>((1.f / (float)setFPS) - frameTime));

		if (enableFPSLimiter)
			std::this_thread::sleep_for(nextFrame);

		FPS = 1 / deltaTime;
		if ((int)counter % 60 == 0)
			logger->log("deltaTime: " + std::to_string(deltaTime * 1000) + "ms | frameTime: " + std::to_string(frameTime * 1000) + "ms | FPS: " + std::to_string(((int)ceil(FPS) + (int)ceil(prevFPS)) / 2));

		prevFPS = FPS;
		counter++;
		fixedTime += deltaTime * 1000;
	}


	// -- Exit application --

	renderer->destroy();
	resourceManager->Dealloc();
	delete uiManager;

	logger->log("Exiting application");

	logger->dumpLog();
}