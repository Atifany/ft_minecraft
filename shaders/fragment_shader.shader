#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D textureData;

void main()
{
	FragColor = texture(textureData, texCoord);
}
