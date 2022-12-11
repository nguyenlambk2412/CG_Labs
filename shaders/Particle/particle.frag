#version 410
in vec2 texCoords;
in float centerDist;
uniform vec3 particleColor;
uniform float particleAge;
uniform sampler2D particleTexture;

out vec4 frag_color;
void main()
{
    // fire base color depends on the distance from original center
    vec3 baseColor = vec3(particleColor.x, particleColor.y * (60.0f - centerDist) / 60.0f, particleColor.z * (60.0f - centerDist) / 60.0f);
	//frag_color = particleColor;
	vec4 sampleColor = texture(particleTexture,texCoords);
    if (sampleColor.a == 0.0f) discard;
	//update the alpha
	sampleColor.a = (sampleColor.a - 0.4f)*particleAge*particleAge;
    // mix basecolor and texture color
	frag_color = vec4(mix(baseColor, sampleColor.rgb, 0.5f), sampleColor.a);
}
