in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D framebufferSampler;

void main()
{
    FragColor = texture(framebufferSampler, TexCoords);
}