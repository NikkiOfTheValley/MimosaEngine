in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D framebufferSampler;

void main()
{
    FragColor = vec4(texture(framebufferSampler, TexCoords).r, texture(framebufferSampler, TexCoords).g, texture(framebufferSampler, TexCoords).b, 1);
}