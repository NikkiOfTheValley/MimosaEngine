#include <chrono>
#include <thread>
#include "globals.h"
#include "rendering/renderer.h"
#include "rendering/types/shader.h"
#include "rendering/types/mesh.h"
#include "input_handler.h"
#include <stb_image.h>
#include "ui/types/text.h"
#include "rendering/resource_manager.h"
#include "ui/ui_manager.h"
#include "rendering/types/camera.h"
#include "rendering/material_manager.h"
#include "rendering/texture_manager.h"
#include "physics/physics_manager.h"
#include "rendering/types/obj_loader.h"
#include "accurate_timer.h"
#include "math/formatting_util.h"

#include "embedded_files/headers/assets_flatShader_vert.h"
#include "embedded_files/headers/assets_flatShader_frag.h"
#include "embedded_files/headers/assets_button_png.h"
#include "embedded_files/headers/assets_button_highlighted_png.h"
#include "embedded_files/headers/assets_text_box_png.h"
#include "embedded_files/headers/assets_text_box_highlighted_png.h"

using namespace math;

// - Global variables -

Logger logger;
uint16_t supportedFeatures;
float fov = 45;
bool inUI = true;
math::vec2 mousePos = math::vec2();
bool lmbPressed = false;
bool useTextInput = false;
std::string* currentlySelectedText = nullptr;

// - Local variables -

// deltaTime & FPS calculation variables

double deltaTime = 0.0f;
double frameTime = 0.0f;
double FPS = 0.0f;
double prevFPS = 0.0f;
uint64_t counter = 0; // Increments once every frame
double fixedTime = 0; // The amount of time since program startup in milliseconds

ui::UIManager* uiManager;
MaterialManager materialManager;
ResourceManager* resourceManager;
PhysicsManager* physicsManager;
bool enableFPSLimiter = true;
bool displayUI = true;
int setFPS = 60;

int main(int /*argc*/, char* /*argv[]*/)
{
	// -- Initialize --

	logger = Logger();

	logger.log("MimosaEngine " + ENG_VERSION_STR);
	logger.log(NAME_STR + " " + VERSION_STR);

	// Check what the CPU supports

	#ifdef _WIN32

	//  Windows
	#define cpuid(info, x)    __cpuidex(info, x, 0)

	#else

	//  GCC Intrinsics
	#include <cpuid.h>
	static void cpuid(int info[4], int InfoType) {
		__cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]);
	}

	#endif

	int info[4];

	cpuid(info, 0);
	int nIds = info[0];

	cpuid(info, 0x80000000);
	unsigned int nExIds = info[0];

	if (nIds >= 0x00000001)
	{
		cpuid(info, 0x00000001);
		if ((info[3] & ((int)1 << 25)) != 0) { supportedFeatures |= HW_SSE; };
		if ((info[3] & ((int)1 << 26)) != 0) { supportedFeatures |= HW_SSE2; };
		if ((info[2] & ((int)1 << 0)) != 0) { supportedFeatures |= HW_SSE3; };

		if ((info[2] & ((int)1 << 28)) != 0) { supportedFeatures |= HW_AVX; };
		if ((info[2] & ((int)1 << 12)) != 0) { supportedFeatures |= HW_FMA3; };
	}

	if (nIds >= 0x00000007)
	{
		cpuid(info, 0x00000007);
		if ((info[1] & ((int)1 << 5)) != 0) { supportedFeatures |= HW_AVX2; };

		if ((info[1] & ((int)1 << 16)) != 0) { supportedFeatures |= HW_AVX512F; };
		if ((info[1] & ((int)1 << 28)) != 0) { supportedFeatures |= HW_AVX512CD; };
		if ((info[1] & ((int)1 << 26)) != 0) { supportedFeatures |= HW_AVX512PF; };
		if ((info[1] & ((int)1 << 27)) != 0) { supportedFeatures |= HW_AVX512ER; };
		if ((info[1] & ((int)1 << 31)) != 0) { supportedFeatures |= HW_AVX512VL; };
		if ((info[1] & ((int)1 << 30)) != 0) { supportedFeatures |= HW_AVX512BW; };
		if ((info[1] & ((int)1 << 17)) != 0) { supportedFeatures |= HW_AVX512DQ; };
		if ((info[2] & ((int)1 << 1)) != 0) { supportedFeatures |= HW_AVX512VBMI; };
	}

	if (nExIds >= 0x80000001)
	{
		cpuid(info, 0x80000001);

		if ((info[3] & ((int)1 << 29)) != 0) { supportedFeatures |= HW_x64; };
		if ((info[2] & ((int)1 << 16)) != 0) { supportedFeatures |= HW_FMA4; };
	}

	// Now check what the OS supports, and zero out the flags that have CPU support, but no OS support

	cpuid(info, 1);

	bool osUsesXSAVE_XRSTORE = info[2] & (1 << 27) || false;

	if (osUsesXSAVE_XRSTORE && ((supportedFeatures & HW_AVX) != 0))
	{
		unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
		bool avxSupportedByOS = (xcrFeatureMask & 0x6) == 0x6;
		bool avx512SupportedByOS = (xcrFeatureMask & 0xe6) == 0xe6;

		if (((supportedFeatures & HW_AVX) != 0) && avxSupportedByOS != true)
		{
			logger.warn("AVX is supported by the CPU, but not the OS! This may cause performace issues!");

			// Zero out all AVX flags
			supportedFeatures &= ~(HW_AVX | HW_AVX2);
		}

		const uint16_t AVX512_BITMASK = HW_AVX512F | HW_AVX512CD | HW_AVX512PF | HW_AVX512ER | HW_AVX512VL | HW_AVX512BW | HW_AVX512DQ | HW_AVX512VBMI;

		if (((supportedFeatures & AVX512_BITMASK) != 0) && avx512SupportedByOS != true)
		{
			logger.warn("AVX512 is supported by the CPU, but not the OS! This may cause performace issues!");

			// Zero out all AVX512 flags
			supportedFeatures &= ~AVX512_BITMASK;
		}
	}

	// Output supported features

	logger.log("Supported Features:");

	std::stringstream output;

	((supportedFeatures & HW_x64) != 0) ? output << "    x64: Yes\n" : output << "    x64: No\n";
	
	((supportedFeatures & HW_SSE) != 0) ? output << "[LOG]:     SSE: Yes\n" : output << "[LOG]:     SSE: No\n";
	((supportedFeatures & HW_SSE2) != 0) ? output << "[LOG]:     SSE2: Yes\n" : output << "[LOG]:     SSE2: No\n";
	((supportedFeatures & HW_SSE3) != 0) ? output << "[LOG]:     SSE3: Yes\n" : output << "[LOG]:     SSE3: No\n";
	
	((supportedFeatures & HW_AVX) != 0) ? output << "[LOG]:     AVX: Yes\n" : output << "[LOG]:     AVX: No\n";
	((supportedFeatures & HW_AVX2) != 0) ? output << "[LOG]:     AVX2: Yes\n" : output << "[LOG]:     AVX2: No\n";
	((supportedFeatures & HW_FMA3) != 0) ? output << "[LOG]:     FMA3: Yes\n" : output << "[LOG]:     FMA3: No\n";
	((supportedFeatures & HW_FMA4) != 0) ? output << "[LOG]:     FMA4: Yes\n" : output << "[LOG]:     FMA4: No\n";
	
	((supportedFeatures & HW_AVX512F) != 0) ? output << "[LOG]:     AVX512 Foundation: Yes\n" : output << "[LOG]:     AVX512 Foundation: No\n";
	((supportedFeatures & HW_AVX512CD) != 0) ? output << "[LOG]:     AVX512 Conflict Detection: Yes\n" : output << "[LOG]:     AVX512 Conflict Detection: No\n";
	((supportedFeatures & HW_AVX512PF) != 0) ? output << "[LOG]:     AVX512 Prefetch: Yes\n" : output << "[LOG]:     AVX512 Prefetch: No\n";
	((supportedFeatures & HW_AVX512ER) != 0) ? output << "[LOG]:     AVX512 Exponential + Reciprocal: Yes\n" : output << "[LOG]:     AVX512 Exponential + Reciprocal: No\n";
	((supportedFeatures & HW_AVX512VL) != 0) ? output << "[LOG]:     AVX512 Vector Length Extensions: Yes\n" : output << "[LOG]:     AVX512 Vector Length Extensions: No\n";
	((supportedFeatures & HW_AVX512BW) != 0) ? output << "[LOG]:     AVX512 Byte + Word: Yes\n" : output << "[LOG]:     AVX512 Byte + Word: No\n";
	((supportedFeatures & HW_AVX512DQ) != 0) ? output << "[LOG]:     AVX512 Doubleword + Quadword: Yes\n" : output << "[LOG]:     AVX512 Doubleword + Quadword: No\n";
	((supportedFeatures & HW_AVX512VBMI) != 0) ? output << "[LOG]:     AVX512 Vector Byte Manipulation Instructions: Yes" : output << "[LOG]:     AVX512 Vector Byte Manipulation Instructions: No";

	logger.log(output.str());

	if (!(supportedFeatures & HW_x64))
		logger.fatal("Hardware is not x64! How is this application even running!?");

	if (!(supportedFeatures & HW_FMA3) && !(supportedFeatures & HW_FMA4))
		logger.fatal("Hardware does not support FMA instructions! This is required to run the physics engine!");

	if (!(supportedFeatures & HW_SSE2))
		logger.fatal("Hardware does not support SSE2! SSE2 is required to run the physics engine!");


	resourceManager = &ResourceManager::getInstance();
	physicsManager = new PhysicsManager();

	InputHandler inputHandler;

	Renderer* renderer = &Renderer::getInstance();

	if (!renderer->init(800, 600, inputHandler.mouse_callback, inputHandler.key_callback))
		return -1;

	TextureManager::getInstance().Init();

	// The UI manager needs to be initialized after the texture manager,
	// as it uses the texture atlas while initializing text rendering
	uiManager = new ui::UIManager();

	// -- Load assets --

	stbi_set_flip_vertically_on_load(true);

	// The flat shader is mandatory for the UI to render correctly
	resourceManager->LoadShader(
		assets_flatShader_vert, ASSETS_FLATSHADER_VERT_SIZE,
		assets_flatShader_frag, ASSETS_FLATSHADER_FRAG_SIZE,
		"flatShader");

	// We need button.png and button-highlighted.png to render buttons
	resourceManager->LoadTexture(assets_button_png, ASSETS_BUTTON_PNG_SIZE, true, "buttonBackground");
	resourceManager->LoadTexture(assets_button_highlighted_png, ASSETS_BUTTON_HIGHLIGHTED_PNG_SIZE, true, "buttonBackgroundHighlighted");

	// We need text-box.png and text-box-highlighted.png to render text boxes
	resourceManager->LoadTexture(assets_text_box_png, ASSETS_TEXT_BOX_PNG_SIZE, true, "textBox");
	resourceManager->LoadTexture(assets_text_box_highlighted_png, ASSETS_TEXT_BOX_HIGHLIGHTED_PNG_SIZE, true, "textBoxHighlighted");

	resourceManager->LoadTexture("assets/example.png", true, "exampleImage");
	resourceManager->LoadTexture("assets/example2.png", true, "exampleImage2");
	resourceManager->LoadShader("assets/example.vert", "assets/example.frag", "exampleShader");

	resourceManager->LoadShader("assets/examplePostProcessingShader.vert", "assets/examplePostProcessingShader.frag", "examplePostProcessingShader");
	renderer->AddNewPostProcessingShader(resourceManager->GetShader("examplePostProcessingShader"));

	// -- Create materials --

	materialManager.CreateMaterial("exampleMaterial", "exampleShader", "exampleImage2");


	// -- Create UIs --

	// First, create a new UI
	uiManager->NewUI("example");

	// Then, select it
	uiManager->SelectUI("example");

	// Then, start creating new elements on that new UI

	uiManager->CreateTextElement("Dynamic Example Text", "example", math::vec2{ 0.05f, 0.066f }, 4);

	uiManager->CreateTextElement("Static Example Text", "example_static", math::vec2{0.05f, 0.15f}, 4, true);

	uiManager->CreateButtonElement("Example Button", math::vec2{ 0.41f, 0.36f }, math::vec2{ 0.38f, 0.08f }, 4,
		[&]() {
			uiManager->UpdateTextElement("example", "Dynamic Example Text 2");
		});

	uiManager->CreateTextBoxElement("Example Text Box", "text_box", math::vec2{ 0.355f, 0.5f }, math::vec2{ 0.38f, 0.06f }, 4);

	uiManager->CreateTextBoxElement("Example Text Box 2", "text_box_2", math::vec2{ 0.355f, 0.58f }, math::vec2{ 0.45f, 0.06f }, 4);

	uiManager->CreateButtonElement("Example Button 2", math::vec2{ 0.355f, 0.7f }, math::vec2{ 0.38f, 0.08f }, 4,
		[&]() {
			std::string text = uiManager->GetTextBoxContents("text_box");

			uiManager->UpdateTextElement("example", text);
		});

	// We have to select a UI before entering the render loop
	uiManager->SelectUI("example");

	inUI = true;


	// -- Create meshes --

	renderer->CreateNewMesh(
		"assets/cube.obj",
		"exampleMesh",
		materialManager.GetMaterial("exampleMaterial"),
		vec3{ 4.f, 5.f, 2.f });

	renderer->CreateNewMesh(
		"assets/cube.obj",
		"exampleMesh2",
		materialManager.GetMaterial("exampleMaterial"),
		vec3{ 4.f, 5.f, 2.f });

	renderer->CreateNewMesh(
		"assets/floor.obj",
		"floor",
		materialManager.GetMaterial("exampleMaterial"),
		vec3{ 4.f, -10.f, 2.f });

	renderer->CreateNewMesh(
		"assets/torus.obj",
		"brokenTorus",
		materialManager.GetMaterial("exampleMaterial"),
		vec3{ 4.f, 4.f, 2.f });
	

	// -- Create physics objects --

	CollisionMesh collisionMeshCube;
	CollisionPrimitive cube;
	cube.type = CollisionPrimitiveType::Cube;
	cube.scale = vec3(0.125f, 0.125f, 0.125f);

	collisionMeshCube.AddPrimitive(cube);

	physicsManager->CreateObject("test", { 4.f, 5.f, 2.001f }, { 0.f, 0.f, 0.f }, DENSITY_WOOD, collisionMeshCube, {}, nullptr, vec3(), vec3(), false);

	physicsManager->CreateObject("test2", { 4.f, 5.f, 3.001f }, { 0.f, 0.f, 0.f }, DENSITY_WOOD, collisionMeshCube);

	CollisionMesh collisionMeshFloor;
	cube.scale = vec3(10.f, 1.f, 10.f);
	collisionMeshCube.AddPrimitive(cube);

	physicsManager->CreateObject("floor", { 4.f, -10.f, 2.f }, { 0.f, 0.f, 0.f }, DENSITY_CAST_IRON, collisionMeshFloor, {}, nullptr, vec3(), vec3(), false, true);

	// -- Create camera --

	Camera cam;
	cam.fov = 90.f;
	cam.pos = vec3{ -10, 0, 0 };
	
	bool lastStateOfF2 = false;
	bool lastStateOfF4 = false;

	// -- Start physics simulation --

	physicsManager->Start();

	// -- Render loop --

	while (!glfwWindowShouldClose(renderer->window))
	{
		auto startOfFrame = std::chrono::steady_clock::now();

		// -- Get input --

		inputHandler.processInput(renderer->window);

		// Reload assets if necessary

		if (inputHandler.reloadAssets)
		{
			logger.log("Reloading assets");
			//resourceManager->ReloadShaders();
			//resourceManager->ReloadTextures();
			materialManager.ReloadMaterials();
			//uiManager->UpdateTextElement("example");
			inputHandler.reloadAssets = false;
		}

		if (glfwGetKey(renderer->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !useTextInput)
			cam.pos.y -= 1.f * (float)deltaTime;
		if (glfwGetKey(renderer->window, GLFW_KEY_SPACE) == GLFW_PRESS && !useTextInput)
			cam.pos.y += 1.f * (float)deltaTime;
		
		vec3 camFront;
		camFront.x = cos(cam.GetRotation().z) * cos(cam.GetRotation().y);
		camFront.y = sin(cam.GetRotation().y);
		camFront.z = sin(cam.GetRotation().z) * cos(cam.GetRotation().y);

		if (glfwGetKey(renderer->window, GLFW_KEY_W) == GLFW_PRESS && !useTextInput)
			cam.pos += camFront * 2.5f * (float)deltaTime;
		if (glfwGetKey(renderer->window, GLFW_KEY_S) == GLFW_PRESS && !useTextInput)
			cam.pos -= camFront * 2.5f * (float)deltaTime;
		if (glfwGetKey(renderer->window, GLFW_KEY_A) == GLFW_PRESS && !useTextInput)
			cam.pos -= normalize(cross(camFront, vec3(0, 1, 0))) * 2.5f * (float)deltaTime;
		if (glfwGetKey(renderer->window, GLFW_KEY_D) == GLFW_PRESS && !useTextInput)
			cam.pos += normalize(cross(camFront, vec3(0, 1, 0))) * 2.5f * (float)deltaTime;

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

		if (lastStateOfF4 == false && glfwGetKey(renderer->window, GLFW_KEY_F4) == GLFW_PRESS)
			displayUI = !displayUI;
		lastStateOfF4 = glfwGetKey(renderer->window, GLFW_KEY_F4) == GLFW_PRESS;

		cam.Update();

		renderer->GetMesh("exampleMesh")->position = physicsManager->GetPhysObject("test")->GetProperties().pos;
		renderer->GetMesh("exampleMesh2")->position = physicsManager->GetPhysObject("test2")->GetProperties().pos;

		renderer->GetMesh("floor")->position = physicsManager->GetPhysObject("floor")->GetProperties().pos;

		glfwPollEvents();

		// deltaTime & FPS calculations
		// Don't ask me how this works. I barely know myself.

		using namespace std::chrono;

		auto endOfFrame = steady_clock::now();
		deltaTime = ((double)duration_cast<microseconds>(endOfFrame - startOfFrame).count() * 0.000001);
		frameTime = deltaTime;
		if (enableFPSLimiter && (1.f / (float)setFPS) > frameTime)
			deltaTime += (1.f / (float)setFPS) - frameTime;
		
		if (enableFPSLimiter)
			BlockForNanoseconds(long long(((1.f / (double)(setFPS)) - frameTime) * conv::SEC_TO_NANOSEC));

		FPS = 1 / deltaTime;
		if ((int)counter % setFPS == 0)
			logger.log(
				"deltaTime: " + math::floatToString((float)deltaTime * conv::SEC_TO_MILLISEC, 4) + "ms "
				"| frameTime: " + math::floatToString((float)frameTime * conv::SEC_TO_MILLISEC, 4) + "ms "
				"| FPS: " + std::to_string(((int)ceil(FPS) + (int)ceil(prevFPS)) / 2));
		

		prevFPS = FPS;
		counter++;
		fixedTime += deltaTime * 1000;
	}


	// -- Exit application --

	physicsManager->Stop();
	renderer->destroy();
	resourceManager->Dealloc();
	delete uiManager;

	logger.log("Exiting application");

	logger.dumpLog();
}