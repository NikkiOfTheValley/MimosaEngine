in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D framebufferSampler;

void main()
{
    FragColor = vec4(texture(framebufferSampler, TexCoords).rgb, 1);
}