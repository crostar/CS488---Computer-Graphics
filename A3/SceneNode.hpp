// Termm-Fall 2020

#pragma once

#include "Material.hpp"
#include "cs488-framework/MeshConsolidator.hpp"


#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

class ShaderProgram;
class Operation;
class OperationStack;

class RenderParams {
public:
	RenderParams(
		const ShaderProgram* shader,
		const glm::mat4 model,
		const glm::mat4 view,
		const BatchInfoMap* batchInfoMap,
		bool isPicking
	) : m_shader(shader), m_model(model),
			m_view(view), m_batchInfoMap(batchInfoMap),
			m_isPicking(isPicking) {}

	const ShaderProgram* m_shader;
	const glm::mat4 m_model;
	const glm::mat4 m_view;
	const BatchInfoMap* m_batchInfoMap;
	bool m_isPicking;
};

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();

	int totalSceneNodes() const;

    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;

    void set_transform(const glm::mat4& m);

    void add_child(SceneNode* child);

    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
		virtual void rotateAtBeginning(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);

	void render(RenderParams params);

	virtual void renderRecur(
		RenderParams params,
		glm::mat4 stackedTrans);

	bool isHeadJoint() {
		return m_name == "head-joint";
	}

	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

	bool isSelected;

    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;

    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;


private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};


class Operation {
public:
	Operation(std::vector<SceneNode*>& nodes) : m_nodes(nodes), m_rotateAngle(0.0f) {}

	Operation(std::vector<SceneNode*>& nodes, glm::vec2 rotateAngle)
		: m_nodes(nodes), m_rotateAngle(rotateAngle) {}

	void execute() {
		transBefore.clear();
		for (auto node : m_nodes) {
			transBefore.push_back(node->trans);
			node->rotateAtBeginning('x', m_rotateAngle.x);
			node->rotateAtBeginning('y', m_rotateAngle.y);
		}
	}

	void undo() {
		for (int i=0; i<m_nodes.size(); i++) {
			m_nodes[i]->trans = transBefore[i];
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

	std::vector<SceneNode*>& getNodes() {
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
		return groupedOperation;
	}

private:
	std::vector<SceneNode*> m_nodes;
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
