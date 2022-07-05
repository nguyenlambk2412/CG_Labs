#version 410

in VS_OUT {
	vec3 texcoord;
} fs_in;

out vec4 frag_color;

uniform samplerCube skyboxTexture;

void main()
{
	frag_color = texture(skyboxTexture, fs_in.texcoord);
}
