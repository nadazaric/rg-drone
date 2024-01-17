#version 330 core

layout(location = 0) in vec2 inPos;
out vec4 chCol;

uniform vec4 uColor;
uniform mat4 uModelMatrix; 

void main()
{
	gl_Position = uModelMatrix * vec4(inPos, 0.0, 1.0);
	chCol = vec4(uColor);
}
