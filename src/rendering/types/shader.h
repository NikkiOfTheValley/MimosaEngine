#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include "../../core/globals.h"
#include <math/vec.h>
#include <math/mat4x4.h>

class Shader
{
public:
	std::string vertPath;
	std::string fragPath;

	Shader(std::string vertShaderPath, std::string fragShaderPath);

	// Bind this shader
	void Bind();

	GLuint GetID();

	// Uniform functions
	void SetBool(const std::string& name, bool value) const;

	void SetInt(const std::string& name, int value) const;

	void SetFloat(const std::string& name, float value) const;

	void SetVec2(const std::string& name, const math::vec2& value) const;

	void SetVec2(const std::string& name, float x, float y) const;

	void SetVec3(const std::string& name, const math::vec3& value) const;

	void SetVec3(const std::string& name, float x, float y, float z) const;

	//void SetVec4(const std::string& name, const vec4& value) const;

	void SetVec4(const std::string& name, float x, float y, float z, float w);

	//void SetMat2(const std::string& name, const mat2& mat) const;

	//void SetMat3(const std::string& name, const mat3& mat) const;

	void SetMat4(const std::string& name, const math::mat4x4f& mat) const;

	void SetFloatArray(const std::string& name, const float* arr, GLsizei size);

	void SetIntArray(const std::string& name, const int* arr, GLsizei size);

	void SetVec2Array(const std::string& name, const math::vec2* arr, GLsizei size);

	void SetVec3Array(const std::string& name, const math::vec3* arr, GLsizei size);

	void SetAny(const std::string& name, const std::any value);

private:
	GLuint ID;

	// Util function for checking shader compilation or linking errors
	void CheckCompileErrors(GLuint shader, std::string type);
};