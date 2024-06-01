#include "renderer.h"
#include "types/vec.h"
#include "types/mesh.h"
#include "types/polygon.h"
#include "types/obj_loader.h"
#include "post_processing_manager.h"

bool Renderer::init(uint32_t width, uint32_t height, std::function<void(GLFWwindow* window, double xpos, double ypos)> cursor_callback, std::function<void(GLFWwindow* window, int key)> key_upd_callback)
{
    Logger* logger = &Logger::getInstance();

    logger->log("Initailizing rendering");

    // Initialize and configure GLFW
    if (!glfwInit())
    {
        logger->err("Failed to initailize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Create window
    window = glfwCreateWindow(width, height, (NAME_STR + " " + VERSION_STR).c_str(), NULL, NULL);
    if (window == NULL)
    {
        logger->err("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    logger->log("Created window");

    glfwMakeContextCurrent(window);
    gladLoadGL();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    cursor_callback_func = cursor_callback;
    key_callback_func = key_upd_callback;

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window,
        [](GLFWwindow* /*window*/, unsigned int codepoint)
        {
            if (useTextInput && currentlySelectedText != nullptr)
            {
                *currentlySelectedText += (char)codepoint;
            }
                
        });

    glfwSetErrorCallback(error_callback);
    
    // Tell GLFW to capture the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    screenDim = new vec2((int)width, (int)height);

    postProcessingManager = new PostProcessingManager();

    // Add the default pass-through shader

    Shader* shader = new Shader("assets/passthrough.vert", "assets/passthrough.frag");
    shader->Bind();
    shader->SetInt("framebufferSampler", 0);

    postProcessingManager->shaders.push_back(shader);

    logger->log("Initialized rendering");
    return true;
}

void Renderer::destroy()
{
    // Clean up GLFW resources
    glfwTerminate();
}

void Renderer::framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{ 
    // Resize the post-processing renderbuffers & textures to match the new window dimensions
    Renderer::getInstance().postProcessingManager->Resize(width, height);

    glViewport(0, 0, width, height);
    *Renderer::getInstance().screenDim = vec2(width, height);
}

void Renderer::error_callback(int error, const char* description)
{
    Logger::getInstance().err("GLFW Error " + std::to_string(error) + "\n" + description);
    glfwSetWindowShouldClose(Renderer::getInstance().window, true);
}

void Renderer::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    try
    {
        Renderer::getInstance().cursor_callback_func(window, xpos, ypos);
    }
    catch (std::bad_function_call e)
    {
        Logger::getInstance().err(e.what());
    }
}

void Renderer::key_callback(GLFWwindow* window, int key, int /*scancode*/, int /*action*/, int /*mods*/)
{
    if (key == GLFW_KEY_BACKSPACE &&
        glfwGetKey(window, key) == GLFW_PRESS &&
        useTextInput && !currentlySelectedText->empty())
    {
        currentlySelectedText->pop_back();
    }
        

    try
    {
        Renderer::getInstance().key_callback_func(window, key);
    }
    catch (std::bad_function_call e)
    {
        Logger::getInstance().err(e.what());
    }
}


// Creates a new mesh either from a .obj file or a std::vector<vert>
void Renderer::CreateNewMesh(
    std::string path,
    std::string name,
    Material* material,
    vec3 pos,
    vec3 rot)
{
    material->RegisterNewMesh(name);
    Mesh* mesh = new Mesh(obj::LoadOBJ(path)[0], material);
    mesh->InitBuffers();
    mesh->position = pos;
    mesh->rotation = rot;

    nameToMeshIndex[name] = meshes.size();
    meshes.push_back(mesh);

    mesh->UpdateMesh();
}

// Creates a new mesh either from a .obj file or a std::vector<vert>
void Renderer::CreateNewMesh(
    std::vector<vert> verts,
    std::string name,
    Material* material,
    vec3 pos,
    vec3 rot)
{
    material->RegisterNewMesh(name);
    Mesh* mesh = new Mesh(verts, material);
    mesh->InitBuffers();
    mesh->position = pos;
    mesh->rotation = rot;

    nameToMeshIndex[name] = meshes.size();
    meshes.push_back(mesh);

    mesh->UpdateMesh();
}

#pragma warning(push)
#pragma warning(disable: 4100)
void Renderer::CreateNewPolygon(std::vector<vert2d> verts, std::string name, Shader* shader, Texture* texture, vec3 pos)
{

}
#pragma warning(pop)

void Renderer::AddNewPostProcessingShader(Shader* shader)
{
    shader->SetInt("framebufferSampler", 0);
    postProcessingManager->shaders.push_back(shader);
}

void Renderer::UpdateMesh(std::string name)
{
    GetMesh(name)->UpdateMesh();
}

void Renderer::UpdatePolygon(std::string name)
{
    GetPolygon(name)->UpdateMesh();
}

Mesh* Renderer::GetMesh(std::string name)
{
    if (!nameToMeshIndex.contains(name))
    {
        Logger::getInstance().err("No mesh exists called " + name);
        return nullptr;
    }

    return meshes[nameToMeshIndex[name]];
}

Polygon2D* Renderer::GetPolygon(std::string name)
{
    if (!nameToPolygonIndex.contains(name))
    {
        Logger::getInstance().err("No polygon exists called " + name);
        return nullptr;
    }

    return polygons[nameToPolygonIndex[name]];
}

void Renderer::Draw(mat4x4f viewMatrix, mat4x4f projectionMatrix)
{
    //postProcessingManager->StartRendering();

    glClearColor(clearColorR, clearColorG, clearColorB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& mesh : meshes)
    {
        mesh->Draw(viewMatrix, projectionMatrix);
    }

    for (auto& poly : polygons)
    {
        poly->Draw();
    }

    //postProcessingManager->EndRendering();
}