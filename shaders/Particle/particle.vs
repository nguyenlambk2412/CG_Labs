#version 410

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

struct ViewProjTransforms
{
	mat4 view_projection;
	mat4 view_projection_inverse;
};
layout (std140) uniform CameraViewProjTransforms
{
	ViewProjTransforms camera;
};

uniform vec3 cameraRightWorld;
uniform vec3 cameraUpWorld;
uniform vec3 particlePos;
uniform vec2 particleSize;



void main()
{
	//vec3 cameraRightWorld	= (vec4(1.0f, 0.0f, 0.0f, 1.0f) * camera.view_projection_inverse).xyz;
	//vec3 cameraUpWorld		= (vec4(0.0f, 1.0f, 0.0f, 1.0f) * camera.view_projection_inverse).xyz;

	//vec3 vertexPosWorld = particlePosition + CameraRight_worldspace * vertex.x * particleSize.x + cameraUpWorld * vertex.y * particleSize.y;
	vec3 vertexPosWorld = 
		particlePos
		+ cameraRightWorld * vertex.x * particleSize.x
		+ cameraUpWorld * vertex.y * particleSize.y;
	//vec3 vertexPosWorld = particlePos + vec3(vertex.xy*particleSize, 0.0f);
    gl_Position = camera.view_projection  * vec4(vertexPosWorld, 1.0f);
}
