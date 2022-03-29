// Termm-Fall 2020

#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	virtual void rotateAtBeginning(char axis, float angle) override;

	virtual void renderRecur(
		RenderParams params,
		glm::mat4 stackedTrans) override;

	struct JointRange {
		double min, init, max;
	};

	glm::vec2 m_rotateAngle;

	JointRange m_joint_x, m_joint_y;
};


class Operation {
public:
	Operation(std::vector<JointNode*>& nodes) : m_nodes(nodes), m_rotateAngle(0.0f) {}

	Operation(std::vector<JointNode*>& nodes, glm::vec2 rotateAngle)
		: m_nodes(nodes), m_rotateAngle(rotateAngle) {}

	void execute() {
		transBefore.clear();
		for (auto node : m_nodes) {
			transBefore.push_back(node->trans);
			rotateBefore.push_back(node->m_rotateAngle);

			node->rotateAtBeginning('x', m_rotateAngle.x);
			node->rotateAtBeginning('y', m_rotateAngle.y);
		}
	}

	void undo() {
		for (int i=0; i<m_nodes.size(); i++) {
			m_nodes[i]->trans = transBefore[i];
			m_nodes[i]->m_rotateAngle = rotateBefore[i];
		}
	}

	void setGroup(size_t group) {
		m_group = group;
	}

	size_t groupID() {
		return m_group;
	}

	void addRotation(glm::vec2 rotateAngle) {
		m_rotateAngle += rotateAngle;
	}

	std::vector<JointNode*>& getNodes() {
		return m_nodes;
	}

	glm::vec2 getRotateAngle() {
		return m_rotateAngle;
	}

	static Operation merge(std::vector<Operation> ops) {
		Operation groupedOperation(ops.front().m_nodes);
		for (auto op : ops) {
			groupedOperation.addRotation(op.getRotateAngle());
		}
		groupedOperation.transBefore = ops.front().transBefore;
		groupedOperation.rotateBefore = ops.front().rotateBefore;
		return groupedOperation;
	}

private:
	std::vector<JointNode*> m_nodes;
	std::vector<glm::vec2> rotateBefore;
	glm::vec2 m_rotateAngle;
	std::vector<glm::mat4> transBefore;
	size_t m_group;
};

class OperationStack {
public:
	OperationStack() { clear(); }
	bool undo();
	bool redo();
	void addOperations(std::vector<Operation>& ops);
	void clear();
	// Compress the most recent
	void compressTop();

	std::vector<Operation> m_operations;
	size_t m_next;
	size_t m_size;
};
