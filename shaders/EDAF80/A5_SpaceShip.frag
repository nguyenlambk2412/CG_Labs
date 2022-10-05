#version 410

uniform mat4 normal_model_to_world;

uniform vec3 light_position;
uniform vec3 camera_position;

uniform int mesh_id;

uniform sampler2D albedoEngTex;
uniform sampler2D ambientEngTex;
uniform sampler2D metalicEngTex;
uniform sampler2D normalEngTex;
uniform sampler2D roughnesssEngTex;

uniform sampler2D albedoBodyTex;
uniform sampler2D ambientBodyTex;
uniform sampler2D metalicBodyTex;
uniform sampler2D normalBodyTex;
uniform sampler2D roughnesssBodyTex;

uniform sampler2D emissEngTex;
uniform sampler2D emissBodyTex;
uniform sampler2D maskEngTex;

uniform float shininess_value;

in VS_OUT {
	vec3 fragPos;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} fs_in;

out vec4 frag_color;

void main()
{
	if(0 == mesh_id)
	{
		frag_color =	texture(albedoBodyTex, fs_in.texCoord) + texture(emissBodyTex, fs_in.texCoord)
						+ texture(ambientBodyTex, fs_in.texCoord)*0.1
						+ texture(roughnesssBodyTex, fs_in.texCoord)*0.1;
						+ texture(metalicBodyTex, fs_in.texCoord) *0.2;
	}
	else if(1 ==mesh_id)
	{
		frag_color =	texture(albedoEngTex, fs_in.texCoord) + texture(emissEngTex, fs_in.texCoord) + texture(maskEngTex, fs_in.texCoord)
						+ texture(ambientEngTex, fs_in.texCoord)*0.1
						+ texture(roughnesssEngTex, fs_in.texCoord)*0.1;
						+ texture(metalicEngTex, fs_in.texCoord) * 0.2;
	}
	else
	{
		frag_color =  vec4(1.0, 0.1, 0.0, 1.0f);
	}

}
