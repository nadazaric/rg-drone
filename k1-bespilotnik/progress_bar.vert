#version 330 core

layout (location = 0) in vec2 inPos;
out vec4 channelCol; 

uniform float uProgress;
uniform float uStartPos;
uniform float uMaxWidth;
uniform vec3 uColor;

void main() {
    if (inPos.x > uStartPos)
        gl_Position = vec4(uStartPos + uMaxWidth * uProgress, inPos.y, 0.0, 1.0);
    else
        gl_Position = vec4(uStartPos, inPos.y, 0.0, 1.0);

    channelCol = vec4(uColor.rgb, 1.0);
}