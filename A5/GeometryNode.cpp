// Term - Winter 2022

#include "GeometryNode.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/BatchInfo.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

void updateShaderUniforms(
	RenderParams params,
	GeometryNode & node,
	glm::mat4 stackedTrans
) {
	if (params.m_isDepth) {
		params.m_shader->enable();
			GLint location = params.m_shader->getUniformLocation("lightSpaceMatrix");
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(params.m_lightSpaceMatrix));
			CHECK_GL_ERRORS;

			location = params.m_shader->getUniformLocation("model");
			mat4 model = params.m_model * stackedTrans * node.trans;
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
			CHECK_GL_ERRORS;

		return;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, node.textureId);

	params.m_shader->enable();
		//-- Set ModelView matrix:
		GLint location = params.m_shader->getUniformLocation("model");
		mat4 model = params.m_model * stackedTrans * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
		CHECK_GL_ERRORS;

		location = params.m_shader->getUniformLocation("view");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(params.m_view));
		CHECK_GL_ERRORS;

		location = params.m_shader->getUniformLocation("viewPos");
		glUniform3fv(location, 1, value_ptr(params.m_viewPos));
		CHECK_GL_ERRORS;

		location = params.m_shader->getUniformLocation("lightPos");
		glUniform3fv(location, 1, value_ptr(params.m_lightPos));
		CHECK_GL_ERRORS;

		location = params.m_shader->getUniformLocation("lightSpaceMatrix");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(params.m_lightSpaceMatrix));
		CHECK_GL_ERRORS;

		if (node.m_name == "sun") {
			location = params.m_shader->getUniformLocation("revertNormal");
			glUniform1f(location, -1.0f);
			CHECK_GL_ERRORS;
		} else {
			location = params.m_shader->getUniformLocation("revertNormal");
			glUniform1f(location, 1.0f);
			CHECK_GL_ERRORS;
		}

		// //-- Set NormMatrix:
		// location = params.m_shader->getUniformLocation("NormalMatrix");
		// mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		// glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		// CHECK_GL_ERRORS;
		//
		// //-- Set Material values:
		// location = params.m_shader->getUniformLocation("material.kd");
		// vec3 kd;
		// if (node.isSelected) {
		// 	kd = glm::vec3( 0.7, 0.1, 0.7 );
		// } else {
		// 	kd = node.material.kd;
		// }
		// glUniform3fv(location, 1, value_ptr(kd));
		// CHECK_GL_ERRORS;
		//
		// location = params.m_shader->getUniformLocation("material.ks");
		// vec3 ks;
		// if (node.isSelected) {
		// 	ks = glm::vec3( 0.0, 0.0, 0.0 );
		// } else {
		// 	ks = node.material.ks;
		// }
		// glUniform3fv(location, 1, value_ptr(ks));
		// CHECK_GL_ERRORS;
		//
		// location = params.m_shader->getUniformLocation("material.shininess");
		// float shininess;
		// if (node.isSelected) {
		// 	shininess = 0.0f;
		// } else {
		// 	shininess = node.material.shininess;
		// }
		// glUniform1f(location, shininess);
		// CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId),
		textureId(0)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::renderRecur(
	RenderParams params,
	glm::mat4 stackedTrans)
{
	glm::mat4 originalTrans = trans;
	// trans = trans * stackedTrans;
	updateShaderUniforms(params, *this, stackedTrans);

	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	BatchInfo batchInfo = params.m_batchInfoMap->at(meshId);

	//-- Now render the mesh:
	// if (m_name == "sun" || m_name == "star" || m_name == "moon") {
		// if (!params.m_isDepth) {
		// 	glFrontFace(GL_CW);
		// 	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		// 	glFrontFace(GL_CCW);
		// }
	// } else {
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	// }
	params.m_shader->disable();

	for (SceneNode * node : children) {
		// node->renderRecur(shader, view, batchInfoMap, trans * stackedTrans);
		node->renderRecur(params, stackedTrans * trans);
	}
}
