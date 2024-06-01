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
		// Check if the texture is an atlas texture
		if (atlasLocation != vec2(-1, -1))
		{
			vec2 flippedTexCoords = vec2(TexCoord.x, (1.0 - (TexCoord.y + 1)));

			// The input texture coordinate is in "local UV space", so we need to convert it to "global UV space" in the texture atlas to render it properly
			vec2 globalTexCoord = (flippedTexCoords / atlasSize) + atlasLocation;

			texColor = texture(texture1, globalTexCoord);
		}
		else
			texColor = texture(texture1, vec2(TexCoord.x, (1.0 - (TexCoord.y + 1))));
	}
	else
		texColor = vec4(1, 0, 1, 1);
	
	// Random nonsense to make sure the `Normals` uniform isn't deleted and doesn't mess up everything
	if (Normals.x == 10000)
		discard;
	
	if(texColor.a < 0.1)
        discard;
	
	//FragColor = vec4((Normals + 1) / 2, 1);
	FragColor = texColor;
}