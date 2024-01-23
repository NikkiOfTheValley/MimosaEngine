#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "core/globals.h"
#include <functional>
#include "types/vec.h"

class Shader;
class Texture;
class Mesh;
class Polygon2D;
class Material;
class PostProcessingManager;
struct vert;
struct vert2d;

// The Renderer is a Singleton because more than one thing accesses it while also needing global state

class Renderer
{
public:
	// Should be thread-safe, magic statics (also known as "dynamic initialization and destruction with concurrency") were implemented in VS 2015
	static Renderer& getInstance()
	{
		static Renderer instance;
		return instance;
	}

	GLFWwindow* window;
	std::function<void(GLFWwindow* window, double xpos, double ypos)> cursor_callback_func;
	std::function<void(GLFWwindow* window, int key)> key_callback_func;
	vec2* screenDim;

	bool init(
		uint32_t width,
		uint32_t height,
		std::function<void(GLFWwindow* window, double xpos, double ypos)> cursor_callback,
		std::function<void(GLFWwindow* window, int key)> key_upd_callback);
	void destroy();

	// Creates a new mesh either from a .obj file or a std::vector<vert>
	void CreateNewMesh(
		std::string path,
		std::string name,
		Material* material,
		vec3 pos = vec3(),
		vec3 rot = vec3());

	// Creates a new mesh either from a .obj file or a std::vector<vert>
	void CreateNewMesh(std::vector<vert> verts,
		std::string name,
		Material* material,
		vec3 pos = vec3(),
		vec3 rot = vec3());
	
	void CreateNewPolygon(std::vector<vert2d> verts, std::string name, Shader* shader, Texture* texture = nullptr, vec3 pos = vec3());

	void AddNewPostProcessingShader(Shader* shader);

	void UpdateMesh(std::string name);
	void UpdatePolygon(std::string name);

	Mesh* GetMesh(std::string name);
	Polygon2D* GetPolygon(std::string name);

	void Draw(mat4x4f viewMatrix, mat4x4f projectionMatrix);


	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;
private:
	Renderer() { window = nullptr; };

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void error_callback(int error, const char* description);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	std::vector<Mesh*> meshes;
	std::unordered_map<std::string, size_t> nameToMeshIndex;

	std::vector<Polygon2D*> polygons;
	std::unordered_map<std::string, size_t> nameToPolygonIndex;

	PostProcessingManager* postProcessingManager;

};