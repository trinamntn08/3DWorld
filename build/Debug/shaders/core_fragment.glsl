#version 410 core
in vec2 TexCoords;

uniform sampler2D texture_diffuse;

layout (location=0) out vec4 FragColor;

void main()
{    
    FragColor = texture(texture_diffuse, TexCoords);
}