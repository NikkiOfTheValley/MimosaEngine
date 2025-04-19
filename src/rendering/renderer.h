#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "core/globals.h"
#include <functional>
#include <math/vec.h>
#include <util/tsqueue.h>

class Shader;
class Texture;
class Mesh;
class Polygon2D;
class Material;
class PostProcessingManager;
struct vert;
struct vert2d;

enum class DebugObjectType
{
	Sphere,
	Vector,
	Cube,
};

struct debug_object
{
	DebugObjectType type;
	size_t id;
	math::vec3 pos;
	math::vec3 size;
	math::vec3 rot;
	bool drawOnTop;
};

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
	math::vec2* screenDim;

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
		math::vec3 pos = math::vec3(),
		math::vec3 rot = math::vec3());

	// Creates a new mesh either from a .obj file or a std::vector<vert>
	void CreateNewMesh(std::vector<vert> verts,
		std::string name,
		Material* material,
		math::vec3 pos = math::vec3(),
		math::vec3 rot = math::vec3());
	
	void CreateNewPolygon(std::vector<vert2d> verts, std::string name, Shader* shader, Texture* texture = nullptr, math::vec3 pos = math::vec3());
	
	/*!
	 * @brief Draws a sphere. Thread safe.
	 * @param id The ID of the debug object. If you draw an object from any thread with the same ID, it will replace the object.
	 * @param pos The position of the sphere
	 * @param scale The scale in the X, Y, and Z axes of the sphere
	 */
	void DrawDebugSphere(size_t id, math::vec3 pos, math::vec3 scale);

	/*!
	 * @brief Draws a vector. Thread safe.
	 * @param id The ID of the debug object. If you draw an object from any thread with the same ID, it will replace the object.
	 * @param pos The origin of the vector
	 * @param vector The vector
	 */
	void DrawDebugVector(size_t id, math::vec3 origin, math::vec3 vector);

	/*!
	 * @brief Draws a cube. Thread safe.
	 * @param id The ID of the debug object. If you draw an object from any thread with the same ID, it will replace the object.
	 * @param pos The position of the cube
	 * @param scale The scale of the cube
	 * @param rot The rotation of the cube
	 */
	void DrawDebugCube(size_t id, math::vec3 pos, math::vec3 scale, math::vec3 rot);

	void AddNewPostProcessingShader(Shader* shader);

	void UpdateMesh(std::string name);
	void UpdatePolygon(std::string name);

	Mesh* GetMesh(std::string name);
	Polygon2D* GetPolygon(std::string name);

	void Draw(math::mat4x4f viewMatrix, math::mat4x4f projectionMatrix);

	Material* debugMaterial = nullptr;

	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;
private:
	Renderer() { window = nullptr; };

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void error_callback(int error, const char* description);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	
	/*!
	 * @brief Draws a debug object
	 * @param obj The object to draw
	 */
	void DrawDebugObject(const debug_object& obj, const math::mat4x4f& viewMatrix, const math::mat4x4f& projectionMatrix);

	std::vector<Mesh*> meshes;
	std::unordered_map<std::string, size_t> nameToMeshIndex;

	std::vector<Polygon2D*> polygons;
	std::unordered_map<std::string, size_t> nameToPolygonIndex;

	tsqueue<debug_object> debugRenderQueue;
	std::vector<Mesh*> debugMeshes;

	PostProcessingManager* postProcessingManager;
};