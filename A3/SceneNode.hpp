// Termm-Fall 2020

#pragma once

#include "Material.hpp"
#include "cs488-framework/MeshConsolidator.hpp"


#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

#include <glm/ext.hpp>

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
		return m_name.find("eye joint") != std::string::npos;
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
