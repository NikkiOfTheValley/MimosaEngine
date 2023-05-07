#pragma once
#include "log.h"

class Shader;
class Texture;
class vec2;
class UIManager;
class ResourceManager;

extern ResourceManager* resourceManager;

extern vec2 mousePos;
extern bool lmbPressed;

extern bool inUI;
extern bool useTextInput;

extern std::string textInput;

const std::string VERSION_STR = "v0.0.1";
const std::string NAME_STR = "Example Application";

const float epsilon = 0.005f;

const float clearColorR = 0.2f;
const float clearColorG = 0.3f;
const float clearColorB = 0.3f;