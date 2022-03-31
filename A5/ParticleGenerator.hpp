// /*
//  * Referencing https://learnopengl.com/In-Practice/2D-Game/Particles
//  */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <vector>

#include "cs488-framework/ShaderProgram.hpp"
#include "ShootingStar.hpp"


struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    float     Life;

    Particle()
      : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

// ParticleGenerator acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(unsigned int amount);
    // update all particles
    void update(float dt, ShootingStar* star, unsigned int newParticles,
      glm::vec3 offset = glm::vec3(0.0f));
    // render all particles
    void draw(const glm::mat4& view, const glm::mat4& projection);
private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
    ShaderProgram shader;
    unsigned int texture;
    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle &particle, ShootingStar* star,
      glm::vec3 offset = glm::vec3(0.0f));
};
