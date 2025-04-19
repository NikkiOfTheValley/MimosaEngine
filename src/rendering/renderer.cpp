#include "renderer.h"
#include <math/vec.h>
#include "types/mesh.h"
#include "types/polygon.h"
#include "types/obj_loader.h"
#include "post_processing_manager.h"
#include <queue>

#include "embedded_files/headers/assets_passthrough_vert.h"
#include "embedded_files/headers/assets_passthrough_frag.h"

#include <ft2build.h>
#include <resource/resource_manager.h>
#include FT_FREETYPE_H

using namespace math;

bool Renderer::init(uint32_t width, uint32_t height, std::function<void(GLFWwindow* window, double xpos, double ypos)> cursor_callback, std::function<void(GLFWwindow* window, int key)> key_upd_callback)
{
    logger.log("Initailizing rendering");

    // Initialize and configure GLFW
    if (!glfwInit())
    {
        logger.err("Failed to initailize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Create window
    window = glfwCreateWindow(width, height, (NAME_STR + " " + VERSION_STR).c_str(), NULL, NULL);
    if (window == NULL)
    {
        logger.err("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    logger.log("Created window");

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
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    screenDim = new vec2((int)width, (int)height);

    postProcessingManager = new PostProcessingManager();

    // Add the default pass-through shader

    //Shader* shader = new Shader(assets_passthrough_vert, ASSETS_PASSTHROUGH_VERT_SIZE, assets_passthrough_frag, ASSETS_PASSTHROUGH_FRAG_SIZE);
    //shader->Bind();
    //shader->SetInt("framebufferSampler", 0);

    //postProcessingManager->shaders.push_back(shader);

    logger.log("Initialized rendering");
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
    logger.err("GLFW Error " + std::to_string(error) + "\n" + description);
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
        logger.err(e.what());
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
        logger.err(e.what());
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

void Renderer::DrawDebugSphere(size_t id, math::vec3 pos, math::vec3 scale)
{
    debug_object obj;

    obj.type = DebugObjectType::Sphere;
    obj.id = id;
    obj.pos = pos;
    obj.size = scale;

    debugRenderQueue.push_back(obj);
}

void Renderer::DrawDebugVector(size_t id, math::vec3 origin, math::vec3 vector)
{
    debug_object obj;

    obj.type = DebugObjectType::Vector;

    obj.type = DebugObjectType::Vector;
    obj.id = id;
    obj.pos = origin;
    obj.size = vector;

    debugRenderQueue.push_back(obj);
}

void Renderer::DrawDebugCube(size_t id, math::vec3 pos, math::vec3 scale, math::vec3 rot)
{
    debug_object obj;

    obj.type = DebugObjectType::Cube;
    obj.id = id;
    obj.pos = pos;
    obj.size = scale;
    obj.rot = rot;

    debugRenderQueue.push_back(obj);
}

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
        logger.err("No mesh exists called " + name);
        return nullptr;
    }

    return meshes[nameToMeshIndex[name]];
}

Polygon2D* Renderer::GetPolygon(std::string name)
{
    if (!nameToPolygonIndex.contains(name))
    {
        logger.err("No polygon exists called " + name);
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

    std::queue<debug_object> objsOnTop;
    while (!debugRenderQueue.empty<debug_object>())
    {
        debug_object obj = debugRenderQueue.pop_back<debug_object>();
        if (obj.drawOnTop)
        {
            objsOnTop.push(obj);
            continue;
        }

        DrawDebugObject(obj, viewMatrix, projectionMatrix);
    }

    while (!objsOnTop.empty())
    {
        const debug_object& obj = objsOnTop.front();
        
        DrawDebugObject(obj, viewMatrix, projectionMatrix);
        objsOnTop.pop();
    }
    

    //postProcessingManager->EndRendering();
}

void Renderer::DrawDebugObject(const debug_object& obj, const math::mat4x4f& viewMatrix,  const math::mat4x4f& projectionMatrix)
{
    // Generate vertices
    std::vector<vert> verts;

    switch (obj.type)
    {
    case DebugObjectType::Sphere:
    {
        constexpr size_t NUM_POINTS_X = 16;
        constexpr size_t NUM_POINTS_Y = 16;

        for (size_t i = 0; i < NUM_POINTS_X; i++)
        {
            for (size_t k = 0; k < NUM_POINTS_Y; k++)
            {
                float stepSizeX = (1.f / (float)NUM_POINTS_X) * M_PI * 2.f;
                float stepSizeY = (1.f / (float)NUM_POINTS_Y) * M_PI * 2.f;

                float tX = ((float)i / (float)NUM_POINTS_X) * M_PI * 2.f;
                float tY = ((float)k / (float)NUM_POINTS_Y) * M_PI * 2.f;

                // First point (0, 0)
                math::vec3 p1;
                p1.x = (obj.size.x * cos(tY)) * cos(tX);
                p1.y = (obj.size.y * cos(tY)) * sin(tX);
                p1.z = obj.size.z * sin(tY);

                // Second point (1, 0)
                math::vec3 p2;
                p2.x = (obj.size.x * cos(tY)) * cos(tX + stepSizeX);
                p2.y = (obj.size.y * cos(tY)) * sin(tX + stepSizeX);
                p2.z = obj.size.z * sin(tY);

                // Third point (0, 1)
                math::vec3 p3;
                p3.x = (obj.size.x * cos(tY + stepSizeY)) * cos(tX);
                p3.y = (obj.size.y * cos(tY + stepSizeY)) * sin(tX);
                p3.z = obj.size.z * sin(tY + stepSizeY);

                // Fourth point (1, 1)
                math::vec3 p4;
                p4.x = (obj.size.x * cos(tY + stepSizeY)) * cos(tX + stepSizeX);
                p4.y = (obj.size.y * cos(tY + stepSizeY)) * sin(tX + stepSizeX);
                p4.z = obj.size.z * sin(tY + stepSizeY);

                // Construct triangles
                verts.push_back(vert{p1.x, p1.y, p1.z, 0.f, 0.f, 0.f, 0.f, 0.f});
                verts.push_back(vert{p3.x, p3.y, p3.z, 0.f, 0.f, 0.f, 0.f, 0.f});
                verts.push_back(vert{p4.x, p4.y, p4.z, 0.f, 0.f, 0.f, 0.f, 0.f});

                verts.push_back(vert{p1.x, p1.y, p1.z, 0.f, 0.f, 0.f, 0.f, 0.f});
                verts.push_back(vert{p4.x, p4.y, p4.z, 0.f, 0.f, 0.f, 0.f, 0.f});
                verts.push_back(vert{p2.x, p2.y, p2.z, 0.f, 0.f, 0.f, 0.f, 0.f});
            }
            
        }
        

        break;
    }
    case DebugObjectType::Vector:
    {
        // Generate the cylinder used for the vector line

        constexpr size_t NUM_CIRCLE_POINTS = 10;
        constexpr float CIRCLE_RADIUS = 0.125f;
        constexpr float ARROWHEAD_LENGTH_FACTOR = 0.1f;

        verts.reserve(NUM_CIRCLE_POINTS * 12);

        for (size_t i = 0; i < NUM_CIRCLE_POINTS; i++)
        {
            float stepSize = 1.f / (float)NUM_CIRCLE_POINTS;
            float t = (float)i / (float)NUM_CIRCLE_POINTS;

            // Generate points on the rim of the cylinder 
            // and connect them as a fan from the center

            float x1 = CIRCLE_RADIUS * cos(t * M_PI * 2.f);
            float y1 = CIRCLE_RADIUS * sin(t * M_PI * 2.f);

            float x2 = CIRCLE_RADIUS * cos((t + stepSize) * M_PI * 2.f);
            float y2 = CIRCLE_RADIUS * sin((t + stepSize) * M_PI * 2.f);

            verts.push_back(vert{0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f});
            verts.push_back(vert{x1,  y1,  0.f, 0.f, 0.f, 0.f, 0.f, 0.f});
            verts.push_back(vert{x2,  y2,  0.f, 0.f, 0.f, 0.f, 0.f, 0.f});

            // Generate the cylinder wall triangles

            float vectorLength = math::magnitude(obj.size);
            float arrowheadLength = vectorLength * ARROWHEAD_LENGTH_FACTOR;
            float cylinderLength = vectorLength - arrowheadLength;

            verts.push_back(vert{x1, y1, 0.f,            0.f, 0.f, 0.f, 0.f, 0.f});
            verts.push_back(vert{x1, y1, cylinderLength, 0.f, 0.f, 0.f, 0.f, 0.f});
            verts.push_back(vert{x2, y2, 0.f,            0.f, 0.f, 0.f, 0.f, 0.f});

            verts.push_back(vert{x2, y2, 0.f,            0.f, 0.f, 0.f, 0.f, 0.f});
            verts.push_back(vert{x1, y1, cylinderLength, 0.f, 0.f, 0.f, 0.f, 0.f});
            verts.push_back(vert{x2, y2, cylinderLength, 0.f, 0.f, 0.f, 0.f, 0.f});

            // Generate the arrowhead triangles

            float x1_ah = x1 * 2.f;
            float y1_ah = y1 * 2.f;
            float x2_ah = x2 * 2.f;
            float y2_ah = y2 * 2.f;
            
            verts.push_back(vert{x2_ah,  y2_ah,  cylinderLength, 0.f, 0.f, 0.f, 0.f, 0.f});
            verts.push_back(vert{x1_ah,  y1_ah,  cylinderLength, 0.f, 0.f, 0.f, 0.f, 0.f});
            verts.push_back(vert{0.f,    0.f,    vectorLength,   0.f, 0.f, 0.f, 0.f, 0.f});
        }

        break;
    }
    case DebugObjectType::Cube:
    {
        verts = {
            vert{-0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f, 0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f, 0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f, 0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f, 0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f, 0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f, 0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f, 0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f, 0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{-0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
            vert{ 0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f},
        };

        for (size_t i = 0; i < verts.size(); i++)
        {
            verts[i].x *= obj.size.x;
            verts[i].y *= obj.size.y;
            verts[i].z *= obj.size.z;
        }
        

        break;
    }
    }

    // If the object ID is outside of the range
    // of the debug meshes vector, then resize it
    // to match
    if ((int)obj.id > (int)(debugMeshes.size()) - 1)
    {
        debugMeshes.resize(obj.id + 2);
    }
    
    // If the debug mesh has not been created, then create it
    if (debugMeshes.at(obj.id) == nullptr)
    {
        Mesh* mesh = new Mesh(verts, debugMaterial);
        debugMeshes[obj.id] = mesh;
    }

    // Now that the object's mesh definitely exists,
    // draw it

    if (obj.type == DebugObjectType::Cube)
        debugMeshes[obj.id]->rotation = obj.rot;
    
    if (obj.type == DebugObjectType::Vector)
    {
        math::vec3 direction = math::normalize(obj.size);
        
        debugMeshes[obj.id]->rotation.x = asin(direction.y);
        debugMeshes[obj.id]->rotation.y = atan2(direction.x, direction.z);
    }
    
    debugMeshes[obj.id]->position = obj.pos;
    debugMeshes[obj.id]->verts = verts;
    debugMeshes[obj.id]->UpdateMesh();
    debugMeshes[obj.id]->Draw(viewMatrix, projectionMatrix);
}