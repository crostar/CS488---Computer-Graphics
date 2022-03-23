// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

class Material {
public:
  virtual ~Material();

  virtual glm::vec3 getKd() = 0;
  virtual glm::vec3 getKs() = 0;
  virtual double getShininess() = 0;

protected:
  Material();
};
