#version 410

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
out vec2 texCoords;
uniform mat4 viewProjMatrix;
uniform vec3 cameraRightWorld;
uniform vec3 cameraUpWorld;
uniform vec3 particlePos;


const vec2 particleSize = vec2(0.5f,0.5f);



void main()
{
	texCoords = vertex.zw;
	vec3 vertexPosWorld = 
		particlePos
		+ normalize(cameraRightWorld) * vertex.x * particleSize.x
		+ normalize(cameraUpWorld) * vertex.y * particleSize.y;
	
    gl_Position = viewProjMatrix * vec4(vertexPosWorld, 1.0f);
	//gl_Position = viewProjMatrix * vec4(vertex.xy*particleSize, 0.0f, 1.0f);
}
