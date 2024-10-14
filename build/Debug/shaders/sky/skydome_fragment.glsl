#version 330 core

in vec2 TexCoords;
in float Height;

out vec4 FragColor;

uniform sampler2D skydome;

uniform vec4 gLowColor = vec4(253.0/256.0, 94.0/256.0, 83.0/256.0, 1.0);
uniform vec4 gHighColor = vec4(21.0/256.0, 40.0/256.0, 82.0/256.0, 1.0);



void main()
{
      vec4 TexColor = texture2D(skydome, TexCoords);

      vec4 SkyColor = mix(gLowColor, gHighColor, Height);

      FragColor = TexColor * 0.7 + SkyColor * 0.3;
  //      FragColor = SkyColor; // Test Height is computed correctly 
 //        FragColor = vec4(TexCoords, 0.0, 1.0);
}
