#version 330 core

layout(location = 0) in vec2 inPos;
out vec4 chCol;

uniform vec2 uPos;
uniform vec3 uColor;

void main()
{
	gl_Position = vec4(inPos + uPos, 0.0, 1.0);
	chCol = vec4(1.0, 0.0, 0.0, 1.0);
}