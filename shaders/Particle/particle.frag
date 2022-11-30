#version 410
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D sprite;

void main()
{
    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);//(texture(sprite, TexCoords) * ParticleColor);
}  
