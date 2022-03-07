// Termm-Fall 2020

#include "SceneNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
	isSelected(false),
	m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans)
{
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	// trans = trans * rot_matrix;
  trans = rot_matrix * trans;

}

//---------------------------------------------------------------------------------------
void SceneNode::rotateAtBeginning(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
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
void SceneNode::scale(const glm::vec3 & amount) {
	trans = glm::scale(amount) * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	trans = glm::translate(amount) * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::render(RenderParams params)
{
	this->renderRecur(params, glm::mat4(1.0f));
}

//---------------------------------------------------------------------------------------
void SceneNode::renderRecur(
  RenderParams params,
  glm::mat4 stackedTrans)
{
  for (SceneNode * node : children) {
    node->renderRecur(params, stackedTrans * trans);
  }
}

//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;
	os << "]";

	return os;
}


//---------------------------------------------------------------------------------------
void OperationStack::addOperations(std::vector<Operation>& ops) {
  cout << "Adding operations of size " << ops.size()
    << ", current next/top: " << m_next << ", " << endl;
  size_t nextGroupId = m_next == 0 ? 0 : m_operations[m_next-1].groupID() + 1;
  Operation groupedOperation = Operation::merge(ops);
  groupedOperation.setGroup(nextGroupId);
  m_operations.insert(std::begin(m_operations) + m_next, groupedOperation);
  m_next += 1;
  m_size = m_next;
  cout << "Updated next/top: " << m_next << ", " << endl;
}

void OperationStack::undo() {
  cout << "Undoing, current next/top: " << m_next << ", " << endl;
  if (m_next == 0) {
    cout << "Cannot undo" << endl;
    return;
  }
  size_t topGroupId = m_operations[m_next-1].groupID();
  for (; m_operations[m_next-1].groupID() == topGroupId; m_next--) {
    m_operations[m_next-1].undo();
  }
  cout << "Updated next/top: " << m_next << ", " << endl;
}

void OperationStack::redo() {
  cout << "Redoing, current next/top: " << m_next << ", " << endl;
  if (m_next == m_size) {
    cout << "Cannot redo" << endl;
    return;
  }
  size_t nextGroupId = m_operations[m_next].groupID();
  for (; m_next != m_size && m_operations[m_next].groupID() == nextGroupId; m_next++) {
    m_operations[m_next].execute();
  }
  cout << "Updated next/top: " << m_next << ", " << endl;
}

void OperationStack::compressTop() {

}

void OperationStack::clear() {
  m_operations.clear();
  m_next = 0;
  m_size = 0;
}
