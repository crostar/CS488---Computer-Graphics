/*
 * Referencing https://learnopengl.com/In-Practice/2D-Game/Particles
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <vector>

#include "cs488-framework/ShaderProgram.hpp"

struct Particle {
    glm::vec3 m_position, m_velocity;
    glm::vec4 m_color;
    float     m_life;

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
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    // update all particles
    void Update(float dt, glm::vec3 loc, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    // render all particles
    void Draw();
private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
    Shader shader;
    Texture2D texture;
    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle &particle, glm::vec3 loc, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};
