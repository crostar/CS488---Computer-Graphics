// Termm--Fall 2020

#pragma once



#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  virtual ~PhongMaterial();

  virtual glm::vec3 getKd() { return m_kd; }
  virtual glm::vec3 getKs() { return m_ks; }
  virtual double getShininess() { return m_shininess; }
private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
};
