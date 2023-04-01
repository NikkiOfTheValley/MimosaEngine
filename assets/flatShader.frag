#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform vec2 texSize;
uniform bool uvIsNDC;

void main()
{
	vec4 texColor = vec4(1.0, 0.0, 1.0, 1.0);
	if (uvIsNDC && texSize != vec2(-1, -1))
		texColor = texture(texture1, vec2(TexCoord.x, (1.0 - (TexCoord.y + 1.0))));
	else if (texSize != vec2(-1, -1))
		texColor = texture(texture1, vec2(TexCoord.x / texSize.x, (1.0 - (TexCoord.y + 1.0)) / texSize.y));

	if(texColor.a < 0.1)
        discard;
	
	FragColor = vec4(texColor.rgb, 1.0);
}