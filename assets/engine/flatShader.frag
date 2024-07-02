#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform vec2 texSize;
uniform bool uvIsNDC;

uniform vec2 atlasLocation;
uniform vec2 atlasSize;

void main()
{
	vec4 texColor = vec4(1.0, 0.0, 1.0, 1.0);

	// Check if a texture actually exists before trying to sample it
	if (texSize != vec2(-1, -1))
	{
		vec2 newUVs = TexCoord;

		if (!uvIsNDC) {
			// If the UVs aren't in NDC, then convert them to NDC
			newUVs = newUVs / texSize;
		}

		newUVs.y = 1 - newUVs.y;

		// Make sure the UVs are in range
		newUVs = vec2(clamp(newUVs.x, 0, 1), clamp(newUVs.y, 0, 1));

		// Check if the texture is an atlas texture
		if (atlasLocation != vec2(-1, -1))
		{
			newUVs = newUVs * (texSize);

			// The input texture coordinate is in "local UV space", so we need to convert it to "global UV space" in the texture atlas to render it properly
			vec2 globalTexCoord = (newUVs / atlasSize) + vec2(atlasLocation.x, atlasLocation.y);

			texColor = texture(texture1, vec2(globalTexCoord.x, globalTexCoord.y));

			//texColor = vec4(atlasLocation.x, atlasLocation.y, 0.0, 1.0);
		}
		else
		{
			texColor = texture(texture1, newUVs);
		}
			

	}

	//if (uvIsNDC && texSize != vec2(-1, -1))
	//	texColor = texture(texture1, vec2(TexCoord.x, (1.0 - (TexCoord.y + 1.0))));
	//else if (texSize != vec2(-1, -1))
	//	texColor = texture(texture1, vec2(TexCoord.x / texSize.x, (1.0 - (TexCoord.y + 1.0)) / texSize.y));

	if(texColor.a < 0.1)
        discard;
	
	FragColor = vec4(texColor.rgb, 1.0);
}