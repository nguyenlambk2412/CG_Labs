/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "particle_generator.h"

ParticleGenerator::ParticleGenerator(GLuint shader, unsigned int amount)
    : particleShader(shader), amount(amount)
{
    this->init();
}

void ParticleGenerator::init()
{
	// set up mesh and attribute properties
	unsigned int VBO;
	float particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->particleVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->particleVAO);
	// fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	//load texture
	this->particleTexture = bonobo::loadTexture2D(config::resources_path("textures/fire.jpeg"));
	

	// create this->amount default particle instances
	for (unsigned int i = 0; i < this->amount; ++i)
	{
		this->particles.push_back(Particle(
			glm::vec3(GetRandom(-POS_X_LIM, POS_X_LIM), 0.0f, GetRandom(-POS_Z_LIM, POS_X_LIM)), //position
			glm::vec3(GetRandom(0.0f, VEL_X_LIM), GetRandom(0.001f, VEL_Y_LIM), GetRandom(0.0f, VEL_Z_LIM)),	//velocity
			glm::vec4(COLOR_RED_VAL, GetRandom(0.1, COLOR_GREEN_VAL), GetRandom(0.1, COLOR_BLUE_VAL), 1.0f),	//color
			0.0f
			));
	}
		

	// get all uniforms locations
	GetParticleUniLocations();
}

void ParticleGenerator::Update(float dt, unsigned int newParticles)
{
	// add new particles 
	for (unsigned int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle]);
	}
	// update all particles
	for (unsigned int i = 0; i < this->amount; ++i)
	{
		Particle& p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// particle is alive, thus update
			p.Position += p.Velocity * dt;
			p.Color -= dt * glm::vec4(0.0001f, 0.0001f,0.0f, 0.0001f);
		}
	}
}

// render all particles
void ParticleGenerator::Draw(glm::mat4 viewProjMatrix)
{
	// use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glUseProgram(particleShader);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(this->particleVAO);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particleTexture);
	glUniform1i(particleUniformLocation.particleTexture, 0);
	
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			//update camera view projection matrix
			glUniformMatrix4fv(particleUniformLocation.viewProjMatrix,1, GL_FALSE, glm::value_ptr(viewProjMatrix));
			// up and right vector of the camera
			glUniform3f(particleUniformLocation.cameraRightWorld,
				viewProjMatrix[0][0],
				viewProjMatrix[1][0],
				viewProjMatrix[2][0]);
			glUniform3f(particleUniformLocation.cameraUpWorld,
				viewProjMatrix[0][1],
				viewProjMatrix[1][1],
				viewProjMatrix[2][1]);
			//update particle position, color, size
			glUniform3fv(particleUniformLocation.particleColor, 1, glm::value_ptr(particle.Color));
			glUniform3fv(particleUniformLocation.particlePos, 1, glm::value_ptr(particle.Position));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	
	// don't forget to reset to default blending mode
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(0u);
}

void ParticleGenerator::GetParticleUniLocations()
{
	particleUniformLocation.viewProjMatrix = glGetUniformLocation(particleShader, "viewProjMatrix");
	particleUniformLocation.cameraRightWorld = glGetUniformLocation(particleShader, "cameraRightWorld");
	particleUniformLocation.cameraUpWorld = glGetUniformLocation(particleShader, "cameraUpWorld");
	particleUniformLocation.particleColor = glGetUniformLocation(particleShader, "particleColor");
	particleUniformLocation.particlePos = glGetUniformLocation(particleShader, "particlePos");
	particleUniformLocation.particleTexture = glGetUniformLocation(particleShader, "particleTexture");
	
}

float ParticleGenerator::GetRandom(float from, float to)
{
	return from + (to-from) * std::rand() / (RAND_MAX + 1.0f);
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
	// first search from last used particle, this will usually return almost instantly
	for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// otherwise, do a linear search
	for (unsigned int i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle)
{
	particle.Position = glm::vec3(GetRandom(-POS_X_LIM, POS_X_LIM), 0.0f, GetRandom(-POS_Z_LIM, POS_X_LIM)); //position
	particle.Velocity = glm::vec3(GetRandom(0.0f, VEL_X_LIM), GetRandom(0.001f, VEL_Y_LIM), GetRandom(0.0f, VEL_Z_LIM));	//velocity
	particle.Color = glm::vec4(COLOR_RED_VAL, GetRandom(0.1, COLOR_GREEN_VAL), GetRandom(0.1, COLOR_BLUE_VAL), 1.0f);	//color
	particle.Life = 5000.0f;// GetRandom(5000.0f, 10000.0f);
	
}
