#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
out float Height;

void main()
{
    vec4 pos = projection * view * model * vec4(aPos, 0.0f);
    gl_Position =vec4(pos.x,pos.y,pos.w,pos.w);
    Height = aPos.y;
    TexCoords = aTexCoords;
}
