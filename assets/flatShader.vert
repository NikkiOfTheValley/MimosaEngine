#version 430 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 pos;
uniform vec2 screenDim;
uniform bool vertIsNDC;
uniform bool posIsNDC;

void main()
{
	if (vertIsNDC)
	{
		gl_Position = vec4((vec2(aPos.x - 0.5, -aPos.y + 0.5) + vec2(pos.x, -pos.y)) * 2, 0.1, 1.0);
	}
	else
	{
		vec2 translatedPos = vec2(0.5, 0.5);
		if (posIsNDC)
			translatedPos = (vec2(aPos.x, -aPos.y) + vec2(pos.x * screenDim.x, -pos.y)) + vec2(-screenDim.x, screenDim.y);
		else
			translatedPos = (vec2(aPos.x, -aPos.y) + vec2(pos.x, -pos.y)) + vec2(-screenDim.x, screenDim.y);

		vec2 correctedPos = translatedPos / screenDim;
			

		gl_Position = vec4(correctedPos, 0.1, 1.0);
	}
	
	TexCoord = aTexCoord;
}