/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#define GLM_FORCE_PURE 1
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "core/helpers.hpp"
#include "config.hpp"


// Represents a single particle and its state
struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    float     Life;

	Particle(glm::vec3 pos, glm::vec3 vec, glm::vec4 col, float lif)
	{
		Position = pos;
		Velocity = vec;
		Color = col;
		Life = lif;
	}
};

struct ParticleShaderLocations
{
	GLuint viewProjMatrix{ 0u };
	GLuint cameraRightWorld{ 0u };
	GLuint cameraUpWorld{ 0u };
	GLuint particlePos{ 0u };
	GLuint particleSize{ 0u };
	GLuint particleColor{ 0u };
	GLuint particleTexture{ 0u };
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(GLuint shader, unsigned int amount);
    // update all particles
    void Update(float dt, unsigned int newParticles);
    // render all particles
	void Draw(glm::mat4 viewProjMatrix);

private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
	GLuint particleShader;
	ParticleShaderLocations particleUniformLocation;
    unsigned int particleVAO;
	GLuint particleTexture;
    // initializes buffer and vertex attributes
    void init();
	// get all uniforms locations
	void GetParticleUniLocations();
	float GetRandom(float from, float to);

	unsigned int firstUnusedParticle();
	void respawnParticle(Particle& particle);

	const float VEL_Y_LIM = 0.01f;
	const float VEL_X_LIM = 0.0002f;
	const float VEL_Z_LIM = 0.0002f;

	const float COLOR_RED_VAL = 1.0F;
	const float COLOR_GREEN_VAL = 0.5F;
	const float COLOR_BLUE_VAL = 0.5F;

	const float POS_X_LIM = 6.0f;
	const float POS_Z_LIM = 6.0f;
};

#endif
