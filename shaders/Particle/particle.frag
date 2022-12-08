#version 410
in vec2 texCoords;
uniform vec3 particleColor;
uniform sampler2D particleTexture;

out vec3 frag_color;
void main()
{
	frag_color = particleColor;
	//frag_color = vec3(texture(particleTexture,texCoords));
}
