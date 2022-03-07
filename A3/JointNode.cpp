// Termm-Fall 2020

#include "JointNode.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
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

//---------------------------------------------------------------------------------------
void JointNode::rotateAtBeginning(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			angle = glm::clamp(
				(double) angle,
				m_joint_x.min - m_rotateAngle.x,
				m_joint_x.max - m_rotateAngle.x);
			m_rotateAngle.x += angle;
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
			angle = glm::clamp(
				(double) angle,
				m_joint_y.min - m_rotateAngle.y,
				m_joint_y.max - m_rotateAngle.y);
			m_rotateAngle.y += angle;
			break;
		case 'z':
			rot_axis = vec3(0,0,1);
	    break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	trans = trans * rot_matrix;
}

//---------------------------------------------------------------------------------------
void JointNode::renderRecur(
	RenderParams params,
	glm::mat4 stackedTrans)
{
	for (SceneNode * node : children) {
		node->renderRecur(params, stackedTrans * trans);
	}
}
