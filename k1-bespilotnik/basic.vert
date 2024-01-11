#version 330 core 

layout(location = 0) in vec2 inPos;
out vec4 channelCol;

uniform vec4 uColor;

void main()
{
	gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
	channelCol = uColor;
}