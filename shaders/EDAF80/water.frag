#version 410

uniform vec3 light_position;
uniform vec3 camera_position;

uniform float time;
uniform samplerCube skybox;
uniform sampler2D normalMap;

uniform mat4 normal_model_to_world;

uniform int use_normal_mapping;
uniform int apply_base_color;
uniform int apply_fresnel;
uniform int apply_reflection;
uniform int apply_refraction;

in VS_OUT {
	vec3 fragPos;
	vec2 texCoords;
	vec3 waveNormal;
	vec3 waveTangent;
	vec3 waveBitangent;
	vec3 surNormal;
	vec3 surTangent;
	vec3 surBinormal;
} fs_in;

out vec4 frag_color;



void main()
{
	vec3 normalVec;
	if(1 == use_normal_mapping)
	{
		// Animated normal mapping
		vec2 texScale = vec2(8,4);
		float normalTime = mod(time, 100.0f);
		vec2 normalSpeed = vec2(-0.05f, 0.0f);

		vec2 normalCoord0 = fs_in.texCoords*texScale + normalTime*normalSpeed;
		vec2 normalCoord1 = fs_in.texCoords*texScale*2 + normalTime*normalSpeed*4;
		vec2 normalCoord2 = fs_in.texCoords*texScale*4 + normalTime*normalSpeed*8;

		vec3 normal_bumpy;
		normal_bumpy  = (texture(normalMap, normalCoord0)*2-vec4(1,1,1,1)).rgb;
		normal_bumpy += (texture(normalMap, normalCoord1)*2-vec4(1,1,1,1)).rgb;
		normal_bumpy += (texture(normalMap, normalCoord2)*2-vec4(1,1,1,1)).rgb;
		normal_bumpy = normalize(normal_bumpy);

	
		mat3 waveTBN = mat3(fs_in.waveTangent,
							fs_in.waveBitangent,
							fs_in.waveNormal);

		mat3 surTBN = mat3 (fs_in.surTangent,
							fs_in.surBinormal,
							fs_in.surNormal);

		//with normal mapping
		normalVec = normalize(vec3(normal_model_to_world*vec4(surTBN * waveTBN * normal_bumpy, 0.0f)));

	}
	else
	{
		//without normal mapping
		normalVec = normalize(vec3(normal_model_to_world*vec4(fs_in.waveNormal,0.0f)));
	}
	
	
	//view vector
	vec3 V = normalize(camera_position - fs_in.fragPos);

	//facing
	float facing = 1- max(dot(V,normalVec),0.0f);

	// reflect
	vec3 reflection = reflect(-V,normalVec);

	// refraction
	vec3 refaction = refract(-V,normalVec,1.33);

	//fresnel terms
	float R0 = 0.02037;
	float fastFresnel = R0 + (1-R0)*pow((1-dot(V,normalVec)),5);

	// base colors
	vec4 colorDeep = vec4(0.0f, 0.0f, 0.1f, 1.0f);
	vec4 colorShallow = vec4(0.0f, 0.5f, 0.5f, 1.0f);

	frag_color = mix(colorDeep, colorShallow, facing)*apply_base_color
				+vec4(texture(skybox, reflection).rgb, 1.0f)*(apply_fresnel==1? fastFresnel: 1)*apply_reflection
				+vec4(texture(skybox, refaction).rgb, 1.0f)*(apply_fresnel==1? (1-fastFresnel) : 1)*apply_refraction;
}
