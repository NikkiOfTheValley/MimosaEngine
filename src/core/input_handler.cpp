#include "input_handler.h"

using namespace math;

InputHandler::InputHandler()
{
	mouse_callback = [&](GLFWwindow* window, double xpos, double ypos)
	{
		mousePos.x = (float)xpos;
		mousePos.y = (float)ypos;

		if (inUI)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	};

	key_callback = [&](GLFWwindow* window, int key)
	{
		if (key == GLFW_KEY_F1 && glfwGetKey(window, key) == GLFW_PRESS)
		{
			reloadAssets = true;
		}
	};
}

// Query GLFW whether relevant keys are pressed/released this frame and react accordingly
void InputHandler::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	lmbPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}