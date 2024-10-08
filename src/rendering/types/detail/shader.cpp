#include "../shader.h"

using namespace math;

Shader::Shader(std::string vertShaderPath, std::string fragShaderPath)
{
	namespace fs = std::filesystem;

	Logger* logger = &Logger::getInstance();

	vertPath = vertShaderPath;
	fragPath = fragShaderPath;

	// - Read the vert shader file & the frag shader file -

	// Verify that the vertex shader and fragment shader files exist and are files
	if (!fs::is_regular_file(vertShaderPath)) { logger->err("File " + vertShaderPath + " does not exist or is not a file"); return; }
	if (!fs::is_regular_file(fragShaderPath)) { logger->err("File " + fragShaderPath + " does not exist or is not a file"); return; }

	// Open vert shader file & frag shader file (binary is fastest)
	std::ifstream if_vert(fs::absolute(vertShaderPath), std::ios::binary);
	std::ifstream if_frag(fs::absolute(fragShaderPath), std::ios::binary);

	// Make sure the files are open
	if (!if_vert) { logger->err("Failed to open vert shader file " + vertShaderPath); return; }
	if (!if_frag) { logger->err("Failed to open frag shader file " + fragShaderPath); return; }

	std::string vertCodeString;
	std::string fragCodeString;

	// Read the vert shader file

	if_vert.seekg(0, std::ios::end); // Jump to the end of the file to determine file size
	vertCodeString.resize(if_vert.tellg()); // Resize string accordingly
	if_vert.seekg(0, std::ios::beg); // Seek to beginning
	if_vert.read(vertCodeString.data(), vertCodeString.size()); // Read the file

	// Close the file
	if_vert.close();


	// Read the frag shader file

	if_frag.seekg(0, std::ios::end); // Jump to the end of the file to determine file size
	fragCodeString.resize(if_frag.tellg()); // Resize string accordingly
	if_frag.seekg(0, std::ios::beg); // Seek to beginning
	if_frag.read(fragCodeString.data(), fragCodeString.size()); // Read the file

	// Close the file
	if_frag.close();


	// - Compile shaders -

	const char* vertShaderCode = vertCodeString.c_str();
	const char* fragShaderCode = fragCodeString.c_str();

	GLuint vert, frag;

	// Compile vert shader
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertShaderCode, NULL);
	glCompileShader(vert);
	CheckCompileErrors(vert, "VERT");

	// Compile frag shader
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragShaderCode, NULL);
	glCompileShader(frag);
	CheckCompileErrors(frag, "FRAG");

	// Compile shader program
	ID = glCreateProgram();
	glAttachShader(ID, vert);
	glAttachShader(ID, frag);
	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");

	// Delete the shaders as they're not needed after linking them to the program
	glDeleteShader(vert);
	glDeleteShader(frag);
}

void Shader::Bind()
{
	glUseProgram(ID);
}

GLuint Shader::GetID()
{
	return ID;
}

void Shader::CheckCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			Logger::getInstance().err("Shader compilation error! Type " + type + "\n" + infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			Logger::getInstance().err("Shader linking error! Type " + type + "\n" + infoLog);
		}
	}
}

void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetVec2(const std::string& name, const math::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value.x);
}

void Shader::SetVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::SetVec3(const std::string& name, const math::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value.x);
}

void Shader::SetVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::SetVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::SetMat4(const std::string& name, const math::mat4x4f& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat.data[0][0]);
}

void Shader::SetFloatArray(const std::string& name, const float* arr, GLsizei size)
{
	glUniform1fv(glGetUniformLocation(ID, name.c_str()), size, arr);
}

void Shader::SetIntArray(const std::string& name, const int* arr, GLsizei size)
{
	glUniform1iv(glGetUniformLocation(ID, name.c_str()), size, arr);
}

void Shader::SetVec2Array(const std::string& name, const math::vec2* arr, GLsizei size)
{
	// Yes, I know this is slow, but I'm fairly sure it isn't possible to set the entire array in a single glUniform call
	for (size_t i = 0; i < size; i++)
		glUniform1fv(glGetUniformLocation(ID, (name + "[" + std::to_string(i) + "]").c_str()), 1, &arr[i].x);
}

void Shader::SetVec3Array(const std::string& name, const math::vec3* arr, GLsizei size)
{
	// Yes, I know this is slow, but I'm fairly sure it isn't possible to set the entire array in a single glUniform call
	for (size_t i = 0; i < size; i++)
		glUniform1fv(glGetUniformLocation(ID, (name + "[" + std::to_string(i) + "]").c_str()), 1, &arr[i].x);
}

void Shader::SetAny(const std::string& name, const std::any value)
{
	// Can't use a switch statement here, if you can think of a way to factor this mess out, please open a PR

	if (value.type() == typeid(bool))
		SetBool(name, std::any_cast<bool>(value));
	else if (value.type() == typeid(int))
		SetInt(name, std::any_cast<int>(value));
	else if (value.type() == typeid(float))
		SetFloat(name, std::any_cast<float>(value));
	else if (value.type() == typeid(math::vec2))
		SetVec2(name, std::any_cast<math::vec2>(value));
	else if (value.type() == typeid(math::vec3))
		SetVec3(name, std::any_cast<math::vec3>(value));
	else if (value.type() == typeid(math::mat4x4f))
		SetMat4(name, std::any_cast<math::mat4x4f>(value));
}