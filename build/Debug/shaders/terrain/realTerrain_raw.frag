#version 330

layout(location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform sampler2D gTextureHeight0;
uniform sampler2D gTextureHeight1;
uniform sampler2D gTextureHeight2;
uniform sampler2D gTextureHeight3;

uniform float gHeight0 = 80.0;
uniform float gHeight1 = 210.0;
uniform float gHeight2 = 250.0;
uniform float gHeight3 = 280.0;

uniform vec3 gReversedLightDir;

vec4 CalcTexColor()
{
    vec4 TexColor;

    float Height = WorldPos.y;

    if (Height < gHeight0) 
    {
       TexColor = texture(gTextureHeight0, TexCoords);
    } 
    else if (Height < gHeight1) 
    {
       vec4 Color0 = texture(gTextureHeight0, TexCoords);
       vec4 Color1 = texture(gTextureHeight1, TexCoords);
       float Delta = gHeight1 - gHeight0;
       float Factor = (Height - gHeight0) / Delta;
       TexColor = mix(Color0, Color1, Factor);
    } 
    else if (Height < gHeight2) 
    {
       vec4 Color0 = texture(gTextureHeight1, TexCoords);
       vec4 Color1 = texture(gTextureHeight2, TexCoords);
       float Delta = gHeight2 - gHeight1;
       float Factor = (Height - gHeight1) / Delta;
       TexColor = mix(Color0, Color1, Factor);
    } 
    else if (Height < gHeight3) 
    {
       vec4 Color0 = texture(gTextureHeight2, TexCoords);
       vec4 Color1 = texture(gTextureHeight3, TexCoords);
       float Delta = gHeight3 - gHeight2;
       float Factor = (Height - gHeight2) / Delta;
       TexColor = mix(Color0, Color1, Factor);
    } else 
    {
       TexColor = texture(gTextureHeight3, TexCoords);
    }
    TexColor = texture(gTextureHeight3, TexCoords);
    return TexColor;
}


void main()
{
    vec4 TexColor = CalcTexColor();
    vec3 Normal_ = normalize(Normal);
    float Diffuse = dot(Normal_, gReversedLightDir);
    Diffuse = max(0.3f, Diffuse);
    FragColor = Color * 6.0f* TexColor * Diffuse;

}