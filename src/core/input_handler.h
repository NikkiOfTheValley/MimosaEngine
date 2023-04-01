#pragma once
#include "../rendering/renderer.h"
#include "../rendering/types/vec.h"
#include "../rendering/types/conversion.h"

class InputHandler
{
public:
	InputHandler();

	// Query GLFW whether relevant keys are pressed/released this frame and react accordingly
	void processInput(GLFWwindow* window);

	// Keeps mouse movement outside of the window when created from causing the camera to move
	bool firstMouse = true;

	bool reloadAssets = false;

	// Mouse move callback
	std::function<void(GLFWwindow* window, double xpos, double ypos)> mouse_callback;

	// Key update callback
	std::function<void(GLFWwindow* window, int key)> key_callback;
};