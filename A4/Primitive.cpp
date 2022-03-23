// Termm--Fall 2020

#include "Primitive.hpp"
#include <iostream>
#include <glm/ext.hpp>

using namespace glm;

bool hitSquare(HitParams& params, glm::vec3 m_pos, glm::vec3 normal, float m_size);

Primitive::~Primitive()
{
}

bool Sphere::hit(HitParams& params) {
  vec3 e_ = params.rayOrigin;
  vec3 d_ = params.rayDirection;
  double discriminant =
    dot(d_, e_) * dot(d_, e_) -
    dot(d_, d_) * (dot(e_, e_) - 1.0f);
  // std::cout << "Discriminant: " << discriminant << std::endl;
  if (discriminant < 0) {
    return false;
  }
  float t1 = (- dot(d_, e_) + sqrt(discriminant)) / dot(d_, d_);
  float t2 = (- dot(d_, e_) - sqrt(discriminant)) / dot(d_, d_);
  // printf("Obtain t1=%.4f, t2=%.4f, d*d=%.4f\n", t1, t2, dot(d_, d_));
  if (t1 < params.t_lower || t1 > params.t_upper) {
    t1 = NO_HIT_T;
  }
  if (t2 < params.t_lower || t2 > params.t_upper) {
    t2 = NO_HIT_T;
  }
  float min_t = min(t1, t2);
  if (min_t != NO_HIT_T && min_t < params.t) {
    params.t = min_t;
    params.hitPoint = e_ + min_t * d_;
    params.normal = normalize(params.hitPoint);
    // params.normal = params.hitPoint;

    return true;
  } else {
    return false;
  }
}

Sphere::~Sphere()
{
}

bool Cube::hit(HitParams& params) {
  bool hit = false;
  hit |= hitSquare(params, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), 1.0f);
  hit |= hitSquare(params, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
  hit |= hitSquare(params, glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 1.0f);
  hit |= hitSquare(params, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
  hit |= hitSquare(params, glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f);
  hit |= hitSquare(params, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
  return hit;
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::hit(HitParams& params) {
  vec3 c_ = m_pos;
  float r_ = m_radius;
  vec3 e_ = params.rayOrigin;
  vec3 d_ = params.rayDirection;
  double discriminant =
    dot(d_, e_ - c_) * dot(d_, e_ - c_) -
    dot(d_, d_) * (dot(e_ - c_, e_ - c_) - pow(r_, 2));
  // std::cout << "Discriminant: " << discriminant << std::endl;
  if (discriminant < 0) {
    return false;
  }
  float t1 = (- dot(d_, e_ - c_) + sqrt(discriminant)) / dot(d_, d_);
  float t2 = (- dot(d_, e_ - c_) - sqrt(discriminant)) / dot(d_, d_);
  // printf("Obtain t1=%.4f, t2=%.4f, d*d=%.4f\n", t1, t2, dot(d_, d_));
  if (t1 < params.t_lower || t1 > params.t_upper) {
    t1 = NO_HIT_T;
  }
  if (t2 < params.t_lower || t2 > params.t_upper) {
    t2 = NO_HIT_T;
  }
  float min_t = min(t1, t2);
  if (min_t != NO_HIT_T && min_t < params.t) {
    params.t = min_t;
    params.hitPoint = e_ + min_t * d_;
    params.normal = normalize(params.hitPoint - c_);
    // params.normal = params.hitPoint;
    return true;
  } else {
    return false;
  }
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::hit(HitParams& params) {
  bool hit = false;
  hit |= hitSquare(params, glm::vec3(m_pos), glm::vec3(0.0f, 0.0f, -1.0f), m_size);
  hit |= hitSquare(params, glm::vec3(m_pos.x, m_pos.y, m_pos.z+m_size), glm::vec3(0.0f, 0.0f, 1.0f), m_size);
  hit |= hitSquare(params, glm::vec3(m_pos), glm::vec3(0.0f, -1.0f, 0.0f), m_size);
  hit |= hitSquare(params, glm::vec3(m_pos.x, m_pos.y+m_size, m_pos.z), glm::vec3(0.0f, 1.0f, 0.0f), m_size);
  hit |= hitSquare(params, glm::vec3(m_pos), glm::vec3(-1.0f, 0.0f, 0.0f), m_size);
  hit |= hitSquare(params, glm::vec3(m_pos.x+m_size, m_pos.y, m_pos.z), glm::vec3(1.0f, 0.0f, 0.0f), m_size);
  return hit;
}

bool hitSquare(HitParams& params, glm::vec3 m_pos, glm::vec3 normal, float m_size) {
  float t = dot(m_pos - params.rayOrigin, normal) / dot(params.rayDirection, normal);
  vec3 intersection = params.rayOrigin + t * params.rayDirection;

  vec3 indicator = vec3(1.0f) - normal * normal;
  vec3 relativeCoor = intersection * indicator - m_pos * indicator;

  bool hit =
    all(greaterThanEqual(relativeCoor, vec3(0.0f))) &&
    all(lessThanEqual(relativeCoor, vec3(m_size))) &&
    t >= params.t_lower &&
    t <= params.t_upper;

  // printf("indicator %s, intersection %s, m_pos %s, relativeCoor %s, hit %d\n",
  //        to_string(indicator).c_str(), to_string(intersection).c_str(),
  //        to_string(m_pos).c_str(), to_string(relativeCoor).c_str(), hit);

  if (hit && t < params.t) {
    params.t = t;
    params.normal = normal;
    params.hitPoint = params.rayOrigin + t * params.rayDirection;
    // if (m_pos != vec3(-200.0f, -225.0f, 100.0f)) {
    //   printf("indicator %s, intersection %s, m_pos %s, relativeCoor %s, hit %d\n",
    //          to_string(indicator).c_str(), to_string(intersection).c_str(),
    //          to_string(m_pos).c_str(), to_string(relativeCoor).c_str(), hit);
    //   printf("Normal %s\n", to_string(normal).c_str());
    // }
  }

  return hit;
}
