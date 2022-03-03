// Termm-Fall 2020

#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	virtual void renderRecur(
		const ShaderProgram& shader,
		const glm::mat4& view,
		const BatchInfoMap& batchInfoMap,
		glm::mat4 stackedTrans) override;

	struct JointRange {
		double min, init, max;
	};


	JointRange m_joint_x, m_joint_y;
};
