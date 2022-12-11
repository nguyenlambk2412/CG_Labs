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
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "core/helpers.hpp"
#include "config.hpp"


const float PARTICLE_LIFE = 1.0f;
const float VEL_Y_LIM_MAX = 120.0f;
const float VEL_Y_LIM_MIN = 80.0f;
const float VEL_X_LIM_MAX = 80.0f;
const float VEL_X_LIM_MIN = 50.0f;
const float VEL_Z_LIM_MAX = 80.0f;
const float VEL_Z_LIM_MIN = 50.0f;

const float COLOR_RED_VAL = 1.0F;
const float COLOR_GREEN_VAL = 0.9F;
const float COLOR_BLUE_VAL = 0.2F;

const float POS_X_LIM = 20.0f;
const float POS_Z_LIM = 20.0f;

const glm::vec3 BASE_COLOR = glm::vec3(1.0f, 1.0f, 0.66f);

// Represents a single particle and its state
struct Particle {
	float OrgPosX;
	float OrgPosZ;
    glm::vec3 Position, Velocity;
    glm::vec3 Color;
    float     Life;

	Particle(glm::vec3 pos, glm::vec3 vec, float lif)
	{
		Position = pos;
		OrgPosX = pos.x;
		OrgPosZ = pos.z;
		Velocity = vec;
        Color = BASE_COLOR;
		//Color = glm::vec3(COLOR_RED_VAL, (VEL_X_LIM - abs(OrgPosX)) / VEL_X_LIM, (VEL_Z_LIM - abs(OrgPosZ)) / VEL_Z_LIM);
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
	GLuint particleAge{ 0u };
    GLuint particleCenter{ 0u };
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
	void Draw(glm::mat4 viewProjMatrix, float tempSize, glm::vec3 camera);

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
    void depthSort(std::map<float, int> &sorted, glm::vec3 camera);

};

#endif
