#version 410

uniform vec3 light_position;
uniform vec3 camera_position;

uniform float t;
uniform samplerCube skybox;
uniform sampler2D normalMap;

in VS_OUT {
	vec3 vertex;
	vec2 normalCoord0;
	vec2 normalCoord1;
	vec2 normalCoord2;
} fs_in;

out vec4 frag_color;

float derivativeX(vec2 position, vec2 direction, float amplitude, float frequency, float phase, float sharpness, float time) 
{ 
	float alpha = sin((direction.x * position.x + direction.y * position.y) * frequency + time * phase) * 0.5 + 0.5;

	return 0.5 * sharpness * frequency * amplitude * pow(alpha, sharpness-1)
			*cos((direction.x * position.x + direction.y * position.y)*frequency + time*phase)
			*direction.x; 
}

float derivativeZ(vec2 position, vec2 direction, float amplitude, float frequency, float phase, float sharpness, float time) 
{ 
	float alpha = sin((direction.x * position.x + direction.y * position.y) * frequency + time * phase) * 0.5 + 0.5;

	return 0.5 * sharpness * frequency * amplitude * pow(alpha, sharpness-1)
			*cos((direction.x * position.x + direction.y * position.y)*frequency + time*phase)
			*direction.y; 
}

void main()
{
	vec4 colorDeep = vec4(0.0f, 0.0f, 0.1f, 1.0f);
	vec4 colorShallow = vec4(0.0f, 0.5f, 0.5f, 1.0f);
	float dx = 0.0f;
	float dz = 0.0f;

	dx = derivativeX(fs_in.vertex.xz, vec2(-1.0, 0.0), 1.0f, 0.2f, 0.5f, 2.0f, t);
	dx += derivativeX(fs_in.vertex.xz, vec2(-0.7, 0.7), 0.5f, 0.4f, 1.3f, 2.0f, t);

	dz = derivativeZ(fs_in.vertex.xz, vec2(-1.0, 0.0), 1.0f, 0.2f, 0.5f, 2.0f, t);
	dz += derivativeZ(fs_in.vertex.xz, vec2(-0.7, 0.7), 0.5f, 0.4f, 1.3f, 2.0f, t);

	vec3 t = normalize(vec3(1, dx, 0));
	vec3 b = normalize(vec3(0, dz, 1));
	vec3 n = normalize(vec3(-dx,1.0f,-dz));

	mat3 TBN = mat3(t.x, t.y, t.z,
					b.x, b.y, b.z,
					n.x, n.y, n.z);

	//Animated normal mapping
	vec3 nbump = vec3(0.0f, 0.0f, 0.0f);
	nbump  = (texture(normalMap, fs_in.normalCoord0)*2-1).rgb;
	//nbump += (texture(normalMap, fs_in.normalCoord1)*2-1).rgb;
	//nbump += (texture(normalMap, fs_in.normalCoord2)*2-1).rgb;
	nbump = normalize(nbump);

	vec3 normalVec = TBN*nbump;

	vec3 V = normalize(camera_position - fs_in.vertex);

	float facing = 1- max(dot(V,normalVec),0);

	// reflect
	vec3 reflection = reflect(-V,normalVec);

	// refraction
	vec3 refaction = refract(-V,normalVec,1.33);

	//fresnel terms
	float R0 = 0.02037;
	float fastFresnel = R0 + (1-R0)*pow((1-dot(V,normalVec)),5);

	frag_color = mix(colorDeep, colorShallow, facing)
				+ vec4(texture(skybox, reflection).rgb, 1.0f)*fastFresnel
				+ vec4(texture(skybox, refaction).rgb, 1.0f)*(1-fastFresnel);
}
