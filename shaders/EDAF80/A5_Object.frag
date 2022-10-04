#version 410

in VS_OUT {
	vec3 fragPos;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} fr_in;

out vec4 frag_color;

uniform mat4 normal_model_to_world;

uniform sampler2D diffTexture;
uniform sampler2D specTexture;
uniform sampler2D normTexture;

uniform vec3 light_position;
uniform vec3 camera_position;

void main()
{
	vec3 lightVec = normalize(light_position - fr_in.fragPos);
	vec3 camVec = normalize(camera_position - fr_in.fragPos);

	vec3 t = normalize(fr_in.tangent);
	vec3 b = normalize(fr_in.bitangent);
	vec3 n = normalize(fr_in.normal);
	mat3 TBN = mat3(t.x, t.y, t.z,
				b.x, b.y, b.z,
				n.x, n.y, n.z);
	vec3 normMap = normalize(vec3((texture(normTexture, fr_in.texCoord)*2 - vec4(1,1,1,1))));
	vec3 norm	= normalize(mat3(normal_model_to_world) * TBN * normMap);

	vec3 ref = reflect(-lightVec, norm);


	float diff = max(dot(lightVec,norm), 0)  ;
	float spec = pow(max(dot(ref, camVec), 0), 10)  ;

	frag_color = vec4(0.3,0.2,0.2,1.0f) + texture(diffTexture, fr_in.texCoord) * diff + texture(specTexture, fr_in.texCoord) * spec;

	//frag_color = vec4(0.2,0.2,0.2,1.0f)*0.2 + texture(diffTexture, fr_in.texCoord)*0.8;

}
