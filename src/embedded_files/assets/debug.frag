#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normals;

uniform sampler2D texture1;
uniform vec2 texSize;

uniform vec2 atlasLocation;
uniform vec2 atlasSize;

void main()
{
	vec4 texColor;
	// Make sure a texture actually exists before trying to sample it
	if (texSize != vec2(-1, -1))
	{
		texColor = texture(texture1, vec2(TexCoord.x, (1.0 - (TexCoord.y + 1))));
	}
	else
		texColor = vec4(0, 1, 0, 0.5);
	
	// Random nonsense to make sure the `Normals` uniform isn't deleted and doesn't mess up everything
	if (Normals.x == 10000)
		discard;
	
	if(texColor.a < 0.1)
        discard;
	
	FragColor = texColor;
}