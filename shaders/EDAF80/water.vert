#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexcoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBinormal;

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;
uniform float time;

out VS_OUT {
	vec3 fragPos;
	vec2 texCoords;
	vec3 waveNormal;
	vec3 waveTangent;
	vec3 waveBitangent;
	vec3 surNormal;
	vec3 surTangent;
	vec3 surBinormal;
} vs_out;

struct WaveParameters
{
	float	amp;
	vec2	dir;
	float	freq;
	float	phas;
	float	shar;
};


float wave(WaveParameters waveparams, vec2 pos, float time) 
{
	float alpha = sin((waveparams.dir.x * pos.x + waveparams.dir.y * pos.y) * waveparams.freq + time * waveparams.phas) * 0.5 + 0.5;
	return waveparams.amp * pow(alpha, waveparams.shar); 
}

float derivative(WaveParameters waveparams, vec2 pos, float time, float direction)
{ 
	float alpha = sin((waveparams.dir.x * pos.x + waveparams.dir.y * pos.y) * waveparams.freq + time * waveparams.phas) * 0.5 + 0.5;

	return 0.5 * waveparams.shar * waveparams.freq * waveparams.amp * pow(alpha, waveparams.shar-1)
			*cos((waveparams.dir.x * pos.x + waveparams.dir.y * pos.y)*waveparams.freq + time*waveparams.phas)
			*direction;
}

void main()
{

//calculate the wave and derivatives
	WaveParameters waveparams[2] = WaveParameters[2](
		WaveParameters(1.0, vec2(-1.0, 0.0), 0.2, 0.5, 2.0),
		WaveParameters(0.5, vec2(-0.7, 0.7), 0.4, 1.3, 2.0)
	);
	
	vec3 displaced_vertex = vertex;
	float dx = 0.0f;
	float dz = 0.0f;
	for (int i = 0; i < 2; i++)
	{
		displaced_vertex.y += wave(waveparams[i], vertex.xz, time);
		dx += derivative(waveparams[i], vertex.xz, time, waveparams[i].dir.x);
		dz += derivative(waveparams[i], vertex.xz, time, waveparams[i].dir.y);
	}
	
//Ouputs	
	vs_out.fragPos = vec3(vertex_model_to_world * vec4(displaced_vertex, 1.0f));
	vs_out.texCoords = aTexcoord.xy;
	vs_out.waveTangent = vec3(1, dx, 0);
	vs_out.waveBitangent = vec3(0, dz, 1);
	vs_out.waveNormal = vec3(-dx,1.0f,-dz);
	vs_out.surNormal = aNormal;
	vs_out.surTangent = aTangent;
	vs_out.surBinormal = aBinormal;

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(displaced_vertex, 1.0);
}



