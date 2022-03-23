#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
class GeometryNode;

const float NO_HIT_T = 1000.0f;

class HitParams {
public:
  HitParams(
    glm::vec3 o, glm::vec3 d, float tu,
    float tl, float t, glm::vec3 n, glm::vec3 p, GeometryNode* node
  ) : rayOrigin(o), rayDirection(d), t_upper(tu), t_lower(tl),
      t(t), normal(n), hitPoint(p), objNode(node) {}

  HitParams(
    glm::vec3 o, glm::vec3 d, float tu, float tl
  ) : rayOrigin(o), rayDirection(d), t_upper(tu), t_lower(tl),
      t(NO_HIT_T), normal(1.0f), hitPoint(0.0f), objNode(nullptr) {}

  HitParams(glm::vec3 o, glm::vec3 d) :
    rayOrigin(o), rayDirection(d), t_upper(1.0f), t_lower(0.0f),
    t(NO_HIT_T), normal(1.0f), hitPoint(0.0f), objNode(nullptr)  {}

  HitParams() :
    rayOrigin(0.0f), rayDirection(0.0f), t_upper(1.0f), t_lower(0.0f),
    t(NO_HIT_T), normal(1.0f), hitPoint(0.0f), objNode(nullptr)  {}

  friend std::ostream& operator<<(std::ostream& out, const HitParams& params);


  glm::vec3 rayOrigin;
  glm::vec3 rayDirection;
  float t_upper;
  float t_lower;
  float t;
  glm::vec3 normal;
  glm::vec3 hitPoint;
  GeometryNode* objNode;
};
