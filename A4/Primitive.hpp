// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>
#include "HitParams.hpp"

class Primitive {
public:
  Primitive(bool h) : isHier(h) {}

  virtual bool hit(HitParams& params) = 0;

  virtual ~Primitive();

  bool isHier;
};

class Sphere : public Primitive {
public:
  Sphere() : Primitive(true) {}

  virtual bool hit(HitParams& params) override;

  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  Cube() : Primitive(true) {}

  virtual bool hit(HitParams& params) override;

  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : Primitive(false), m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();

  virtual bool hit(HitParams& params) override;

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : Primitive(false), m_pos(pos), m_size(size)
  {
  }

  virtual ~NonhierBox();

  virtual bool hit(HitParams& params) override;

private:
  glm::vec3 m_pos;
  double m_size;
};
