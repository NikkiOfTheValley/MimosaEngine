#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D framebufferSampler;

void main()
{
    FragColor = vec4(texture(framebufferSampler, TexCoords).r, texture(framebufferSampler, TexCoords).g / 2, texture(framebufferSampler, TexCoords).b / 2, 0);
}