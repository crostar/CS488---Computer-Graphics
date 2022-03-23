// Termm--Fall 2020

#include "JointNode.hpp"
using namespace glm;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

bool JointNode::hit(HitParams& params) {
	// return SceneNode::hit(params);
	params.rayOrigin = vec3(get_inverse() * vec4(params.rayOrigin, 1));
	params.rayDirection = vec3(get_inverse() * vec4(params.rayDirection, 1));

	bool hit = false;

	for (auto child : children) {
		hit |= child->hit(params);
	}

	// if (hit) {
		params.hitPoint = vec3(get_transform() * vec4(params.hitPoint, 1));
		// params.normal = glm::mat3(get_inverse()) * params.normal;
		params.normal = normalize(glm::mat3(get_inverse()) * params.normal);
	// }

	params.rayOrigin = vec3(get_transform() * vec4(params.rayOrigin, 1));
	params.rayDirection = vec3(get_transform() * vec4(params.rayDirection, 1));

	return hit;
}
