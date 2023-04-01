#include "../camera.h"

void Camera::Update()
{
	double xOffset = mousePos.data[0] - prevMousePos.data[0];
	double yOffset = prevMousePos.data[1] - mousePos.data[1]; // Reversed since y-coordinates go from bottom to top
	prevMousePos.data[0] = mousePos.data[0];
	prevMousePos.data[1] = mousePos.data[1];

	float sensitivity = 0.001f;

	// Don't move the camera if in a UI
	if (inUI)
		sensitivity = 0.f;

	xOffset *= sensitivity;
	yOffset *= sensitivity;

	rot.data[2] += (float)xOffset;
	rot.data[1] += (float)yOffset;

	// Make sure that when pitch is +/- 90 degrees, screen doesn't get flipped
	if (rot.data[1] > (float)conv::deg_to_rad(89.0f))
		rot.data[1] = (float)conv::deg_to_rad(89.0f);
	if (rot.data[1] < (float)conv::deg_to_rad(-89.0f))
		rot.data[1] = (float)conv::deg_to_rad(-89.0f);

	vec3 front;
	front.data[0] = cos(rot.data[2]) * cos(rot.data[1]);
	front.data[1] = sin(rot.data[1]);
	front.data[2] = sin(rot.data[2]) * cos(rot.data[1]);

	int w, h;
	glfwGetWindowSize(Renderer::getInstance().window, &w, &h);

	projMatrix.perspective((float)w / (float)h, 0.01f, 1000.0f, fov);
	viewMatrix.lookAt(pos, pos + normalize(front), vec3(0.0f, 1.0f, 0.0f));
}

mat4x4f Camera::GetViewMatrix() { return viewMatrix; }

mat4x4f Camera::GetProjectionMatrix() { return projMatrix; }

vec3 Camera::GetRotation() { return rot; }