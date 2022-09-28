#version 410
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexcoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform bool use_normal_mapping;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	vec2 texCoord;
} vs_out;


void main()
{
	if(false == use_normal_mapping)
	{
		vs_out.normal = vec3(normal_model_to_world *  vec4(aNormal, 1.0));
	}
	else
	{
		vs_out.normal = aNormal;
		vs_out.tangent = aTangent;
		vs_out.bitangent = aBitangent;
	}



	vs_out.fragPos = vec3(vertex_model_to_world *  vec4(vertex, 1.0));
	vs_out.texCoord = aTexcoord.xy;

	gl_Position = vertex_world_to_clip * vertex_model_to_world *  vec4(vertex, 1.0);
}
