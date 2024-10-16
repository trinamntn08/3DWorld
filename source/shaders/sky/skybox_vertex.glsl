#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
    vec4 pos = projection * view * model * vec4(aPos, 1.0f);
    gl_Position =vec4(pos.x,pos.y,pos.w,pos.w); // force z = 1.0 to always render at depth = 1.0
    TexCoords = aPos;
}