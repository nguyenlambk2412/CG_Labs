#version 410
in vec2 texCoords;
uniform vec3 particleColor;
uniform float particleAge;
uniform sampler2D particleTexture;

out vec4 frag_color;
void main()
{
	//frag_color = particleColor;
	vec4 sampleColor = texture(particleTexture,texCoords);
	//update the alpha
	sampleColor.a = (sampleColor.a - 0.2f)*particleAge*particleAge;
	if(sampleColor.a < 0.1f) discard;
	frag_color = sampleColor;
}
