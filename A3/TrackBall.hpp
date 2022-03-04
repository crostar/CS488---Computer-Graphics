#pragma once

#include <glm/glm.hpp>

using namespace glm;

class TrackBall {
public:
  TrackBall() : m_rotMat(1.0f) {}

  void update(vec2 fNew, vec2 fOld, float fDiameter);
  void reset();

  mat4 m_rotMat;

private:
  vec3 calcRotVec(vec2 fNew, vec2 fOld, float fDiameter);
  void updateAxisRotMatrix(vec3 fVec);
};
