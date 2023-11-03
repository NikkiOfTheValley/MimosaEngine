#include "post_processing_manager.h"
#include "renderer.h"

PostProcessingManager::PostProcessingManager()
{
	Renderer* renderer = &Renderer::getInstance();
	int screenWidth = (int)renderer->screenDim->x;
	int screenHeight = (int)renderer->screenDim->y;

	// Initialize framebuffers and renderbuffers
	glGenFramebuffers(1, &multisampledFramebuffer);
	glGenFramebuffers(1, &framebuffer);
	glGenRenderbuffers(1, &renderBuffer);

	// Initialize renderbuffer with a multisampled color buffer
	glBindFramebuffer(GL_FRAMEBUFFER, multisampledFramebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);

	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);

	// Make sure the multisampled framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Logger::getInstance().err("Failed to initialize multisampled framebuffer");

	// Initialize the texture to blit multisampled color buffer to
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	// Make sure the framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Logger::getInstance().err("Failed to initialize framebuffer");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Initialize the quad that will be drawn on to create the post-processing effects 

	unsigned int VBO;
	float vertices[] = {
		 // XY      // UV
		-1.f, -1.f, 0.f, 0.f,
		-1.f,  1.f, 0.f, 1.f,
		 1.f,  1.f, 1.f, 1.f,

		-1.f, -1.f, 0.f, 0.f,
		 1.f,  1.f, 1.f, 1.f,
		 1.f, -1.f, 1.f, 0.f
	};
	glGenVertexArrays(1, &framebufferVertsVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(framebufferVertsVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

PostProcessingManager::~PostProcessingManager()
{
	glDeleteFramebuffers(1, &multisampledFramebuffer);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteRenderbuffers(1, &renderBuffer);
	glDeleteVertexArrays(1, &framebufferVertsVAO);
	glDeleteTextures(1, &framebufferTexture);
}

void PostProcessingManager::Resize(int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// Called when rendering starts
void PostProcessingManager::StartRendering()
{
	glBindFramebuffer(GL_FRAMEBUFFER, multisampledFramebuffer);
	glClearColor(clearColorR, clearColorG, clearColorB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

// Called when rendering ends
void PostProcessingManager::EndRendering()
{
	Renderer* renderer = &Renderer::getInstance();
	int screenWidth = (int)renderer->screenDim->x;
	int screenHeight = (int)renderer->screenDim->y;

	// Copy multisampled color buffer into intermediate framebuffer so that texture holds the color information
	glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(framebufferVertsVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	std::reverse(shaders.begin(), shaders.end());
	for (auto& shader : shaders)
	{
		shader->Bind();
		shader->SetInt("framebufferSampler", 0);

		// render textured quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	std::reverse(shaders.begin(), shaders.end());
	glBindVertexArray(0);
}