#include "Skybox.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.hpp>

#include <string>
#include <iostream>
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/GlErrorCheck.hpp"


float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

Skybox::Skybox() {}

Skybox::~Skybox() {
  glDeleteVertexArrays(1, &m_vao);
  glDeleteBuffers(1, &m_vbo);
}

void Skybox::init() {
  // Create shader
  m_shader.generateProgramObject();
  m_shader.attachVertexShader( CS488Window::getAssetFilePath("skybox/skybox.vs").c_str() );
  m_shader.attachFragmentShader( CS488Window::getAssetFilePath("skybox/skybox.fs").c_str() );
  m_shader.link();

  // Load VAO and VBO
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  // Restore to default
  CHECK_GL_ERRORS;
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Load skybox texture
  std::vector<std::string> faces
  {
      // CS488Window::getAssetFilePath("skybox/right.jpg"),
      // CS488Window::getAssetFilePath("skybox/left.jpg"),
      // CS488Window::getAssetFilePath("skybox/top.jpg"),
      // CS488Window::getAssetFilePath("skybox/bottom.jpg"),
      // CS488Window::getAssetFilePath("skybox/front.jpg"),
      // CS488Window::getAssetFilePath("skybox/back.jpg")
      // CS488Window::getAssetFilePath("skybox/sqpantsrt.jpg"),
      // CS488Window::getAssetFilePath("skybox/sqpantslf.jpg"),
      // CS488Window::getAssetFilePath("skybox/sqpantsup.jpg"),
      // CS488Window::getAssetFilePath("skybox/sqpantsdn.jpg"),
      // CS488Window::getAssetFilePath("skybox/sqpantsft.jpg"),
      // CS488Window::getAssetFilePath("skybox/sqpantsbk.jpg")
      CS488Window::getAssetFilePath("skybox/right.png"),
      CS488Window::getAssetFilePath("skybox/left.png"),
      CS488Window::getAssetFilePath("skybox/top.png"),
      CS488Window::getAssetFilePath("skybox/bot.png"),
      CS488Window::getAssetFilePath("skybox/front.png"),
      CS488Window::getAssetFilePath("skybox/back.png")
  };
  loadTexture(faces);
}

void Skybox::render(const glm::mat4& viewMatrix, const glm::mat4& projMatrix) {
  m_shader.enable();
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    glm::mat4 view = glm::mat4(glm::mat3(viewMatrix)); // remove translation from the view matrix
    //-- Set ModelView matrix:
    GLint location = m_shader.getUniformLocation("ModelView");
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(view));
    CHECK_GL_ERRORS;
    //-- Set Perspective matrix:
    location = m_shader.getUniformLocation("Perspective");
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(projMatrix));
    CHECK_GL_ERRORS;
    //-- Set skybox texture
    location = m_shader.getUniformLocation("skybox");
    glUniform1i(location, 0);
    CHECK_GL_ERRORS;

    // skybox cube
    glBindVertexArray(m_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

  m_shader.disable();
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
void Skybox::loadTexture(std::vector<std::string> faces)
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    // stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
          // std::cout << "Cubemap texture load at path: " << faces[i] << width << height << nrChannels << std::endl;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // stbi_set_flip_vertically_on_load(false);
}
