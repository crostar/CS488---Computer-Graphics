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
	// mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	// trans = trans * rot_matrix;
}

//---------------------------------------------------------------------------------------
void JointNode::renderRecur(
	RenderParams params,
	glm::mat4 stackedTrans)
{
	mat4 rot_matrix = glm::rotate(degreesToRadians(m_rotateAngle.x), vec3(1.0f, 0.0f, 0.0f));
	rot_matrix *= glm::rotate(degreesToRadians(m_rotateAngle.y), vec3(0.0f, 1.0f, 0.0f));
	for (SceneNode * node : children) {
		node->renderRecur(params, stackedTrans * trans * rot_matrix);
	}
}


//---------------------------------------------------------------------------------------
void OperationStack::addOperations(std::vector<Operation>& ops) {
  // std::cout << "Adding operations of size " << ops.size()
  //   << ", current next/top: " << m_next << ", " << std::endl;
  size_t nextGroupId = m_next == 0 ? 0 : m_operations[m_next-1].groupID() + 1;
  Operation groupedOperation = Operation::merge(ops);
  groupedOperation.setGroup(nextGroupId);
  m_operations.insert(std::begin(m_operations) + m_next, groupedOperation);
  m_next += 1;
  m_size = m_next;
  // std::cout << "Updated next/top: " << m_next << ", " << std::endl;
}

bool OperationStack::undo() {
  // std::cout << "Undoing, current next/top: " << m_next << ", " << std::endl;
  if (m_next == 0) {
    // std::cout << "Cannot undo" << std::endl;
    return false;
  }
  size_t topGroupId = m_operations[m_next-1].groupID();
  for (; m_operations[m_next-1].groupID() == topGroupId; m_next--) {
    m_operations[m_next-1].undo();
  }
  // std::cout << "Updated next/top: " << m_next << ", " << std::endl;
  return true;
}

bool OperationStack::redo() {
  // std::cout << "Redoing, current next/top: " << m_next << ", " << m_size << std::endl;
  if (m_next == m_size) {
    // std::cout << "Cannot redo" << std::endl;
    return false;
  }
  size_t nextGroupId = m_operations[m_next].groupID();
  for (; m_next != m_size && m_operations[m_next].groupID() == nextGroupId; m_next++) {
    m_operations[m_next].execute();
  }
  // std::cout << "Updated next/top: " << m_next << ", " << std::endl;
  return true;
}

void OperationStack::clear() {
  while (undo()) {
    continue;
  }
  m_operations.clear();
  m_next = 0;
  m_size = 0;
}
