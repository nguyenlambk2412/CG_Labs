#version 410

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
out vec2 texCoords;
out float centerDist;
uniform mat4 viewProjMatrix;
uniform vec3 cameraRightWorld;
uniform vec3 cameraUpWorld;
uniform vec3 particlePos;
uniform vec2 particleOrgCenter;
uniform float particleSize;



void main()
{
	texCoords = vertex.zw;
    centerDist = distance(particlePos.xy, particleOrgCenter);
	vec3 vertexPosWorld = 
		particlePos
		+ normalize(cameraRightWorld) * vertex.x * particleSize
		+ normalize(cameraUpWorld) * vertex.y * particleSize;
	
    gl_Position = viewProjMatrix * vec4(vertexPosWorld, 1.0f);
	//gl_Position = viewProjMatrix * vec4(vertex.xy*particleSize, 0.0f, 1.0f);
}
