#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normals;

out vec2 TexCoord;
out vec3 Normals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0f);
	TexCoord = texCoord;

	// The normals must be in world space
	Normals = (vec4(normals, 1.0f)).xyz;
}