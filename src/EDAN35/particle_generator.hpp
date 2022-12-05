//
//  particle_generator.hpp
//  EDAN35_Assignment2
//
//  Created by TianXN on 2022/12/3.
//

#ifndef particle_generator_hpp
#define particle_generator_hpp

#include <stdio.h>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "core/node.hpp"
#include "core/helpers.hpp"



// Represents a single particle and its state
struct Particle {
    Node node;
    glm::vec3 Position{0.0}, Color{0.0};
    glm::vec2 Size{0.0};
    float Velocity{0.0}, Life{0.0};
};


// ParticleGenerator acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator();
    ParticleGenerator(bonobo::mesh_data const& shape, glm::vec3 color, glm::vec3 pos, glm::vec2 size, unsigned int amount);
    std::vector<Particle> GetParticles();
    bonobo::mesh_data GetShape();
    // update all particles
    void Update(float dt, unsigned int newParticles);
    // render all particles
//    void Draw(GLuint const* program, ViewProjTransforms camera_view_proj_transforms);
private:
    // state
    std::vector<Particle> particles;
    bonobo::mesh_data shape;
    unsigned int amount;
    glm::vec3 particleColor;
    glm::vec3 particlePos;
    glm::vec2 particleSize;

    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle &particle);
};

#endif /* particle_generator_hpp */
