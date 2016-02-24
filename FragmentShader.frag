#version 410
in vec2 vTexCoord;
out vec4 out_color;
uniform sampler2D noiseTexture;
void main()
{
 out_color = texture(noiseTexture, vTexCoord).rrrr;
 out_color.a = 1;
}