#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform float gMinHeight = 0.0f;
uniform float gMaxHeight = 356.0f;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 Color;
out vec2 TexCoords;
out vec3 Normal;
out vec3 WorldPos;

void main()
{
    gl_Position = projection * view * model * vec4(Position, 1.0);
    TexCoords = aTexCoords;   
    Normal= aNormal;
    WorldPos = Position;

    float DeltaHeight = gMaxHeight - gMinHeight;
    float HeightRatio = (Position.y - gMinHeight) / DeltaHeight;
    float c = HeightRatio * 0.8 + 0.2;
    Color = vec4(c, c, c, 1.0);
}
