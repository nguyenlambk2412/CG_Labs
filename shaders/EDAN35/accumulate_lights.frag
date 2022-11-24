#version 410

struct ViewProjTransforms
{
	mat4 view_projection;
	mat4 view_projection_inverse;
};

layout (std140) uniform CameraViewProjTransforms
{
	ViewProjTransforms camera;
};

layout (std140) uniform LightViewProjTransforms
{
	ViewProjTransforms lights[4];
};

uniform int light_index;

uniform sampler2D depth_texture;
uniform sampler2D normal_texture;
uniform sampler2D shadow_texture;

uniform vec2 inverse_screen_resolution;

uniform vec3 camera_position;

uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 light_direction;
uniform float light_intensity;
uniform float light_angle_falloff;

layout (location = 0) out vec4 light_diffuse_contribution;
layout (location = 1) out vec4 light_specular_contribution;


void main()
{
	vec2 shadowmap_texel_size = 1.0f / textureSize(shadow_texture, 0);

	//construct texture coordinate
	vec2 texCoord = gl_FragCoord.xy*inverse_screen_resolution;
	vec4 testColor = texture(depth_texture, texCoord);
	float depth = texture(depth_texture, texCoord).r; 
	//retrieve fragment position
	vec4 temp = vec4(texCoord,depth, 1.0f)*2-1;
	temp = camera.view_projection_inverse*temp;	//transform back to world coordinate
	temp = temp/temp.w;
	vec3 fragPos = temp.xyz;

	//sample the normal vector
	vec3 normalVec = normalize(vec3(texture(normal_texture, texCoord)*2-1));
	//construct neccessary vectors
	vec3 lightVec = normalize(light_position - fragPos);
	vec3 viewVec = normalize(camera_position - fragPos);
	vec3 reflectVec = reflect(-lightVec, normalVec);
	//calculate diffuse and specular parameters
	float diffuse = max(0,dot(lightVec,normalVec));
	float specular = pow(max(0,dot(reflectVec,viewVec)),30);	//hard code shininess value

	///Distance Falloff calculation 
	float ObjectLightDistance = length(light_position - fragPos);
	float distanceFalloff = 1/pow(ObjectLightDistance,2);

	///Angular Falloff calculation
	float angularFalloff = (dot(normalize(light_direction),-lightVec) - cos(light_angle_falloff))/(1-cos(light_angle_falloff));
	///Final light intensity:
	float finalLightIntensity = light_intensity*distanceFalloff*angularFalloff;

	//Combination of all  lighting effects
	light_diffuse_contribution  = finalLightIntensity*diffuse*vec4(light_color,1.0f);
	light_specular_contribution = finalLightIntensity*specular*vec4(0.9, 0.9, 0.9, 1.0);
}
