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
#include "../rendering/texture_manager.h"
#include "../physics/physics_manager.h"
#include "../rendering/types/obj_loader.h"
#include "accurate_timer.h"

// - Global variables -

uint16_t supportedFeatures;
float fov = 45;
bool inUI = true;
vec2 mousePos = vec2();
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

UIManager* uiManager;
MaterialManager materialManager;
ResourceManager* resourceManager;
PhysicsManager* physicsManager;
Logger* logger;
bool enableFPSLimiter = true;
bool displayUI = true;
int setFPS = 60;

int main(int /*argc*/, char* /*argv[]*/)
{
	// -- Initialize --

	logger = &Logger::getInstance();

	logger->log("MimosaEngine " + ENG_VERSION_STR);
	logger->log(NAME_STR + " " + VERSION_STR);

	// Check what the CPU supports

	#ifdef _WIN32

	//  Windows
	#define cpuid(info, x)    __cpuidex(info, x, 0)

	#else

	//  GCC Intrinsics
	#include <cpuid.h>
		void cpuid(int info[4], int InfoType) {
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
			logger->warn("AVX is supported by the CPU, but not the OS! This may cause performace issues!");

			// Zero out all AVX flags
			supportedFeatures &= ~(HW_AVX | HW_AVX2);
		}

		const uint16_t AVX512_BITMASK = HW_AVX512F | HW_AVX512CD | HW_AVX512PF | HW_AVX512ER | HW_AVX512VL | HW_AVX512BW | HW_AVX512DQ | HW_AVX512VBMI;

		if (((supportedFeatures & AVX512_BITMASK) != 0) && avx512SupportedByOS != true)
		{
			logger->warn("AVX512 is supported by the CPU, but not the OS! This may cause performace issues!");

			// Zero out all AVX512 flags
			supportedFeatures &= ~AVX512_BITMASK;
		}
	}

	// Output supported features

	logger->log("Supported Features:");

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

	logger->log(output.str());

	TextureManager::getInstance().Init();
	resourceManager = &ResourceManager::getInstance();
	uiManager = new UIManager();
	physicsManager = new PhysicsManager();

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

	resourceManager->LoadShader("assets/examplePostProcessingShader.vert", "assets/examplePostProcessingShader.frag", "examplePostProcessingShader");
	renderer->AddNewPostProcessingShader(resourceManager->GetShader("examplePostProcessingShader"));

	Image* img = new Image("assets/font.png", true);

	uint32_t* data = new uint32_t[16 * 16];

	std::memset(data, 0, 16 * 16 * sizeof(uint32_t));


	Image* dest = new Image(data, 16, 16, true);

	img->Blit(dest, 16, 0, 16, 8, 0, 0);

	Texture* tex = new Texture(dest, true);


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

	//uiManager->CreateImageElement(resourceManager->GetTexture("exampleImage"), false, vec2{ 0.25f, 0.1f }, vec2{ 0.25f, 0.25f });

	uiManager->CreateImageElement(tex, false, vec2{ 0.25f, 0.2f }, vec2{ tex->w / renderer->screenDim->x, tex->h / renderer->screenDim->y });

	uiManager->CreateButtonElement("Example Button", vec2{ 0.41f, 0.36f }, vec2{ 0.38f, 0.08f }, 4,
		[&]() {
			uiManager->UpdateTextElement("example", "Dynamic Example Text 2");
		});

	uiManager->CreateTextBoxElement("Example Text Box", "text_box", vec2{ 0.355f, 0.5f }, vec2{ 0.28f, 0.06f }, 4);

	uiManager->CreateTextBoxElement("Example Text Box 2", "text_box_2", vec2{ 0.355f, 0.58f }, vec2{ 0.28f, 0.06f }, 4);

	uiManager->CreateButtonElement("Example Button 2", vec2{ 0.355f, 0.7f }, vec2{ 0.38f, 0.08f }, 4,
		[&]() {
			std::string text = uiManager->GetTextBoxContents("text_box");

			uiManager->UpdateTextElement("example", text);
		});

	// We have to select a UI before entering the render loop
	uiManager->SelectUI("example");

	inUI = true;


	// -- Create meshes --

	renderer->CreateNewMesh(
		"assets/torus.obj",
		"exampleMesh",
		materialManager.GetMaterial("exampleMaterial"),
		vec3{ 4.f, 0.f, 2.f });

	// We have to update the mesh before drawing it, otherwise the mesh
	// data is never sent to the GPU
	renderer->UpdateMesh("exampleMesh");
	

	// -- Create physics objects --

	CollisionMesh collisionMesh;
	collisionMesh.LoadFromOBJ("assets/torus.obj");

	CollisionConstraint* constraint = new CollisionConstraint();
	constraint->collisionMesh = collisionMesh;

	physicsManager->CreateObject("test", { 4.f, 0.f, 2.f }, { 0.f, 0.f, 0.f }, DENSITY_CAST_IRON, constraint, {}, { 0, 1, 0 }, {}, false);

	// -- Create camera --

	Camera cam;
	cam.fov = 90.f;
	cam.pos = vec3{ 0, 0, 0 };
	
	bool lastStateOfF2 = false;


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

		if (glfwGetKey(renderer->window, GLFW_KEY_F4) == GLFW_PRESS)
			displayUI = !displayUI;

		cam.Update();

		renderer->GetMesh("exampleMesh")->rotation.y += 0.5f * (float)deltaTime;

		renderer->GetMesh("exampleMesh")->position = physicsManager->GetPhysObject("test")->GetProperties().pos;

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
			BlockForNanoseconds(long long(((1.f / (double)(setFPS)) - frameTime) * 1000000000));

		FPS = 1 / deltaTime;
		if ((int)counter % setFPS == 0)
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