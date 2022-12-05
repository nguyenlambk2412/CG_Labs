//
//  particle_generator.cpp
//  EDAN35_Assignment2
//
//  Created by TianXN on 2022/12/3.
//

#include "particle_generator.hpp"

ParticleGenerator::ParticleGenerator()
{
    this->init();
}

ParticleGenerator::ParticleGenerator(bonobo::mesh_data const& shape, glm::vec3 color, glm::vec3 pos, glm::vec2 size, unsigned int amount)
    : particleColor(color), particlePos(pos), particleSize(size), shape(shape), amount(amount)
{
    this->init();
}
std::vector<Particle> ParticleGenerator::GetParticles()
{
    return this->particles;
}

bonobo::mesh_data ParticleGenerator::GetShape()
{
    return this->shape;
}
void ParticleGenerator::Update(float dt, unsigned int newParticles)
{
    // add 'newParticles' numbers of new particles
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle]);
    }
    // update all particles
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        Particle &p = this->particles[i];
        p.Life -= dt; // reduce life
        if (p.Life > 0.0f)
        {    // particle is alive, thus update
            p.Position += p.Velocity * dt * glm::vec3(1.0f, 5.0f, 1.0f); // only update position on Y axis, to make effect like blaze rising up
        }
    }
}

// render all particles
//void ParticleGenerator::Draw(GLuint const* cameraRightWorld, GLuint const* cameraUpWorld, GLuint const* program)
//{
//    // use additive blending to give it a 'glow' effect
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//    this->shader.Use();
//    for (Particle particle : this->particles)
//    {
//        if (particle.Life > 0.0f)
//        {
//            
//            particle.node.set_program(program);
//            //glm::mat4 = glm::normalize(&camera_view_proj_transforms.view_projection);
//            glUniform3f(cameraRightWorld, camera_view_proj_transforms.view_projection[0][0],
//                camera_view_proj_transforms.view_projection[1][0],
//                camera_view_proj_transforms.view_projection[2][0]);
//            glUniform3f(cameraUpWorld, camera_view_proj_transforms.view_projection[0][1],
//                camera_view_proj_transforms.view_projection[1][1],
//                camera_view_proj_transforms.view_projection[2][1]);
//            //glUniform3fv(fire_shader_locations.cameraRightWorld, 1, glm::value_ptr(particleColor));
//            //glUniform3fv(fire_shader_locations.cameraUpWorld, 1, glm::value_ptr(particleColor));
//            glUniform3fv(fire_shader_locations.ParticleColor,1, this->particleColor);
//            glUniform3fv(fire_shader_locations.particlePos, 1, this->particlePos);
//            glUniform2fv(fire_shader_locations.particleSize, 1, this->particleSize);
//            //auto const vertex_model_to_world = glm::mat4(1.0f);
//            //glUniformMatrix4fv(fire_shader_locations.vertex_model_to_world, 1, GL_FALSE, glm::value_ptr(vertex_model_to_world));
//            glDisable(GL_CULL_FACE);
//            fire.render(view_projection,    //view matrix
//                        glm::mat4(1.0f),    //world matrix
//                        render_fire_shader,    //shader program
//                        set_uniforms);        //set uniform
//            glEnable(GL_CULL_FACE);
//            glUseProgram(0u);
//            
//            this->shader.SetVector2f("offset", particle.Position);
//            this->shader.SetVector4f("color", particle.Color);
//            this->texture.Bind();
//            glBindVertexArray(this->VAO);
//            glDrawArrays(GL_TRIANGLES, 0, 6);
//            glBindVertexArray(0);
//        }
//    }
//    // don't forget to reset to default blending mode
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//}

void ParticleGenerator::init()
{

    // initialize the particles vector with class attributes
    // create this->amount default particle instances
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        struct Particle particle = { .Position = particlePos, .Color = particleColor, .Size = particleSize, .Velocity = 0.0, .Life = 0.0};
        this->particles.push_back(particle);
    }
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    // first search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i){
        if (this->particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i){
        if (this->particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle)
{
    glm::vec3 random = glm::vec3(((rand() % 100) - 50) / 10.0f, 0.0f, ((rand() % 100) - 50) / 10.0f); // generater random position at X and Z axis
    particle.Position = this->particlePos + random;
    particle.Life = 1.0f; // initialize Life with 1.0
    particle.Velocity = 1.0f; // initialize Velocity with 1.0
}
