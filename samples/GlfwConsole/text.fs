#version 450 core
out vec4 FragColor;

in vec2 texcoord;
layout(binding = 0) uniform sampler2D tex;
layout(location = 0) uniform vec4 color;
void main()
{
	FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).r) * color;
}