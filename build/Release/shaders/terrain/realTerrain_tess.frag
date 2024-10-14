#version 410 core

in float Height;
in vec4 Normal;
in vec2 texCoords;
in float realHeight;

uniform sampler2D gTextureHeight0;
uniform sampler2D gTextureHeight1;
uniform sampler2D gTextureHeight2;
uniform sampler2D gTextureHeight3;

uniform float gHeight0 = 80.0;
uniform float gHeight1 = 210.0;
uniform float gHeight2 = 250.0;
uniform float gHeight3 = 280.0;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out float HeightData;

uniform vec3 gReversedLightDir;

vec4 CalcTexColor()
{
    vec4 TexColor;

    if (Height < gHeight0) 
    {
       TexColor = texture(gTextureHeight0, texCoords*5+5);
    } 
    else if (Height < gHeight1) 
    {
       vec4 Color0 = texture(gTextureHeight0, texCoords*5+5);
       vec4 Color1 = texture(gTextureHeight1, texCoords*5+5);
       float Delta = gHeight1 - gHeight0;
       float Factor = (Height - gHeight0) / Delta;
       TexColor = mix(Color0, Color1, Factor);
    } 
    else if (Height < gHeight2) 
    {
       vec4 Color0 = texture(gTextureHeight1, texCoords*5+5);
       vec4 Color1 = texture(gTextureHeight2, texCoords*5+5);
       float Delta = gHeight2 - gHeight1;
       float Factor = (Height - gHeight1) / Delta;
       TexColor = mix(Color0, Color1, Factor);
    } 
    else if (Height < gHeight3) 
    {
       vec4 Color0 = texture(gTextureHeight2, texCoords*5+5);
       vec4 Color1 = texture(gTextureHeight3, texCoords*5+5);
       float Delta = gHeight3 - gHeight2;
       float Factor = (Height - gHeight2) / Delta;
       TexColor = mix(Color0, Color1, Factor);
    } else 
    {
       TexColor = texture(gTextureHeight3, texCoords*5+5);
    }

    TexColor = texture(gTextureHeight3, texCoords*5+5);
    return TexColor;
}
void main()
{
    float c = (Height + 16)/64.0f;
  //    float c = Height * 0.8 + 0.2;

    vec4 TexColor = CalcTexColor();
    vec3 Normal_ = normalize(Normal.xyz);
    float Diffuse = dot(Normal_, gReversedLightDir);
    Diffuse = max(0.3f, Diffuse);
    vec4 Color = vec4(c, c, c, 1.0);
    FragColor = Color * 8.0f* TexColor * Diffuse;
    HeightData= realHeight;
}