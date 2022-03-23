#include "HitParams.hpp"
#include "GeometryNode.hpp"

std::ostream& operator<<(std::ostream& out, const HitParams& params) {
  std::string nodeName = "None";
  if (params.objNode != nullptr) {
    nodeName = params.objNode->m_name;
  }
  return out << "ray: " << glm::to_string(params.rayOrigin) << ", "
    << glm::to_string(params.rayDirection)
    << "t: " << params.t << " normal: " << glm::to_string(params.normal)
    << " hitpoint: " << glm::to_string(params.hitPoint)
    << "node: " << nodeName << std::endl;
}
