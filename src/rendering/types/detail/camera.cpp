#include "../camera.h"

void Camera::Update()
{
	double xOffset = mousePos.x - prevMousePos.x;
	double yOffset = prevMousePos.y - mousePos.y; // Reversed since y-coordinates go from bottom to top
	prevMousePos.x= mousePos.x;
	prevMousePos.y = mousePos.y;

	float sensitivity = 0.001f;

	// Don't move the camera if in a UI
	if (inUI)
		sensitivity = 0.f;

	xOffset *= sensitivity;
	yOffset *= sensitivity;

	rot.z += (float)xOffset;
	rot.y += (float)yOffset;

	// Make sure that when pitch is +/- 90 degrees, screen doesn't get flipped
	if (rot.y > (float)conv::deg_to_rad(89.0f))
		rot.y = (float)conv::deg_to_rad(89.0f);
	if (rot.y < (float)conv::deg_to_rad(-89.0f))
		rot.y = (float)conv::deg_to_rad(-89.0f);

	vec3 front;
	front.x = cos(rot.z) * cos(rot.y);
	front.y = sin(rot.y);
	front.z = sin(rot.z) * cos(rot.y);

	int w, h;
	glfwGetWindowSize(Renderer::getInstance().window, &w, &h);

	projMatrix.perspective((float)w / (float)h, 0.01f, 1000.0f, fov);
	viewMatrix.lookAt(pos, pos + normalize(front), vec3(0.0f, 1.0f, 0.0f));
}

mat4x4f Camera::GetViewMatrix() { return viewMatrix; }

mat4x4f Camera::GetProjectionMatrix() { return projMatrix; }

vec3 Camera::GetRotation() { return rot; }