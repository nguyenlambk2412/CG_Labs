#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
	vec3 fragPos;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;	
} vs_out;


void main()
{
	vs_out.fragPos = vec3(vertex_model_to_world *  vec4(vertex, 1.0));
	vs_out.texCoord = aTexCoords;
	vs_out.normal = aNormal;
	vs_out.tangent = aTangent;
	vs_out.bitangent = aBitangent;

	gl_Position = vertex_world_to_clip * vertex_model_to_world *  vec4(vertex, 1.0);
}
