#pragma once
#include <vector>
#include "types/shader.h"

class PostProcessingManager
{
public:
	PostProcessingManager();

	~PostProcessingManager();
	
	void Resize(int width, int height);

	// Called when rendering starts
	void StartRendering();

	// Called when rendering ends
	void EndRendering();

	std::vector<Shader*> shaders;

private:
	GLuint multisampledFramebuffer;
	GLuint renderBuffer;

	GLuint framebuffer;
	GLuint framebufferTexture;

	GLuint framebufferVertsVAO;
};