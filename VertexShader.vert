#version 410 
layout(location=0) in vec4 Position; 
layout(location=1) in vec2 TexCoord; 
out vec2 vTexCoord; 
uniform mat4 ProjectionView; 
uniform sampler2D noiseTexture;
uniform float heightscale = 10; 
void main() {  vTexCoord = TexCoord; float height = texture(noiseTexture, TexCoord).r; gl_Position = ProjectionView * (Position + vec4(0, height * heightscale,0,0)) ; } 