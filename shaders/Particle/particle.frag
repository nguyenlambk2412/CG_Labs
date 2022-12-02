#version 410

uniform vec3 ParticleColor;
out vec3 frag_color;
void main()
{
	frag_color = ParticleColor;
}
