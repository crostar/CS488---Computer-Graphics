// Termm-Fall 2020

#pragma once

#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	virtual void renderRecur(
		RenderParams params,
		glm::mat4 stackedTrans) override;

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;
	unsigned int textureId;
};
