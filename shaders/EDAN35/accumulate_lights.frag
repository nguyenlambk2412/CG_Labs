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
	//construc texture coordinate
	vec2 texCoord = vec2(gl_FragCoord.x*inverse_screen_resolution.x, gl_FragCoord.y*inverse_screen_resolution.y);
	//sample the normal vector
	vec3 normalVec = normalize(vec3(texture(normal_texture, texCoord)*2-1));
	//retrieve fragment position
	vec4 temp = texture(depth_texture, texCoord);
	temp = camera.view_projection_inverse*temp;
	vec3 fragPos = temp.xyz/temp.w;

	vec3 lightVec = normalize(light_position - fragPos);
	vec3 viewVec = normalize(camera_position - fragPos);
	float diffuse = max(0,dot(lightVec,normalVec));

	vec3 reflectVec = reflect(-lightVec, normalVec);
	float specular = pow(max(0,dot(reflectVec,normalVec)),50);	//hard code shininess value
	

	light_diffuse_contribution  = diffuse*vec4(0.5, 0.0, 0.1, 1.0);
	light_specular_contribution = specular*vec4(0.9, 0.9, 0.9, 1.0);
}
