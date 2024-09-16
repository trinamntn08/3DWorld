#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform float gMinHeight = 0.0f;
uniform float gMaxHeight = 356.0f;

out vec4 Color;
out vec2 TexCoord;

out vec3 WorldPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoords;

    WorldPos = aPos;

    float DeltaHeight = gMaxHeight - gMinHeight;
    float HeightRatio = (aPos.y - gMinHeight) / DeltaHeight;
    float c = HeightRatio * 0.8 + 0.2;
    Color = vec4(c, c, c, 1.0);
}