// Termm-Fall 2020

#include "GeometryNode.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/BatchInfo.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include <glm/gtc/type_ptr.hpp>


using namespace glm;

void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode& node,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
	}
	shader.disable();
}

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::renderRecur(
	const ShaderProgram& shader, const glm::mat4& view,
	const BatchInfoMap& batchInfoMap, glm::mat4 stackedTrans)
{
	glm::mat4 originalTrans = trans;
	trans = stackedTrans * trans;
	updateShaderUniforms(shader, *this, view);
	trans = originalTrans;

	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	BatchInfo batchInfo = batchInfoMap.at(meshId);

	//-- Now render the mesh:
	shader.enable();
	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	shader.disable();

	for (SceneNode * node : children) {
		node->renderRecur(shader, view, batchInfoMap, stackedTrans * trans);
	}
}
