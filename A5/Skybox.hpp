/*
 *  Referencing https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <vector>

#include "cs488-framework/ShaderProgram.hpp"


class Skybox {
public:
  Skybox();
  ~Skybox();

  void init();

  // Render to current framebuffer
  void render(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

  unsigned int getTexture() { return m_texture; }
  void bindTexture() { glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture); }
  
private:
  void loadTexture(std::vector<std::string> faces);

  unsigned int m_vao, m_vbo;
  unsigned int m_texture;
  ShaderProgram m_shader;
};
