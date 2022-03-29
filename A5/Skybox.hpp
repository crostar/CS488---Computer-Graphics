/*
 *  Referencing https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src
 */
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>


class Skybox {
public:
  Skybox();
  ~Skybox();

  // Render to current framebuffer
  render(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

private:
  void loadTexture(vector<std::string> faces);

  unsigned int m_vao, m_vbo;
  unsigned int m_texture;
};
