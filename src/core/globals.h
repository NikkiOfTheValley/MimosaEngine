#pragma once
#include "log.h"

class vec2;

extern vec2 mousePos;
extern bool lmbPressed;

extern bool inUI;
extern bool useTextInput;

extern std::string textInput;

const std::string ENG_VERSION_STR = "v0.2.0";
const std::string VERSION_STR = "v0.0.1";
const std::string NAME_STR = "Example Application";

const float epsilon = 0.005f;

const float clearColorR = 0.2f;
const float clearColorG = 0.3f;
const float clearColorB = 0.3f;