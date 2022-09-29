#version 410

layout (location = 0) in vec3 vertex; 

uniform mat4 vertex_model_to_world; 
uniform mat4 normal_model_to_world; 
uniform mat4 vertex_world_to_clip;

uniform float t;

out VS_OUT {
	vec3 vertex;
	vec3 normalCoord0;
	vec3 normalCoord1;
	vec3 normalCoord2;
} vs_out;

float wave(vec2 position, vec2 direction, float amplitude, float frequency, float phase, float sharpness, float time) 
{
	float alpha = sin((direction.x * position.x + direction.y * position.y) * frequency + time * phase) * 0.5 + 0.5;
	return amplitude * pow(alpha, sharpness); 
}


void main()
{
	vec3 displaced_vertex = vertex;
	displaced_vertex.y += wave(vertex.xz, vec2(-1.0f, 0.0f), 1.0f, 0.2f, 0.5f, 2.0f, t);
	displaced_vertex.y += wave(vertex.xz, vec2(-0.7f, 0.7f), 0.5f, 0.4f, 1.3f, 2.0f, t); 
	vs_out.vertex = vec3(vertex_model_to_world * vec4(displaced_vertex, 1.0f));

	// animated normal mapping
	vec2 texScale = vec2(8,4);
	float normalTime = mod(t, 100.0f);
	vec2 normalSpeed = vec2(-0.05f, 0.0f);

	//vs_out.normalCoord0.xy = ;
	//vs_out.normalCoord1.xy = ;
	//vs_out.normalCoord2.xy = ;

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(displaced_vertex, 1.0); 
}



