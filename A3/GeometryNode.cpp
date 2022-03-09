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
	RenderParams params,
	GeometryNode & node,
	glm::mat4 stackedTrans
) {
	params.m_shader->enable();

	//-- Set ModelView matrix:
	GLint location = params.m_shader->getUniformLocation("ModelView");
	mat4 modelView = params.m_view * stackedTrans * node.trans;
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
	CHECK_GL_ERRORS;

	if( params.m_isPicking ) {
		uint id = node.m_nodeId;
		// float r = float(id&0xff) / 255.0f;
		float r = float(id&0xff) / 255.0f;
		float g = float((id>>8)&0xff) / 255.0f;
		float b = float((id>>16)&0xff) / 255.0f;

		// std::cout << "ID: " << node.m_nodeId << " rgb: ("
		//  	<< r*255.0f << ", " << g*255.0f << "," << b*255.0f
		// 	<< ")" << std::endl;

		location = params.m_shader->getUniformLocation("material.kd");
		glUniform3f( location, r, g, b );
		CHECK_GL_ERRORS;
	} else {
		//-- Set NormMatrix:
		location = params.m_shader->getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;

		//-- Set Material values:
		location = params.m_shader->getUniformLocation("material.kd");
		vec3 kd;
		if (node.isSelected) {
			kd = glm::vec3( 0.7, 0.7, 0.7 );
		} else {
			kd = node.material.kd;
		}
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;

		location = params.m_shader->getUniformLocation("material.ks");
		vec3 ks;
		if (node.isSelected) {
			ks = glm::vec3( 0.0, 0.0, 0.0 );
		} else {
			ks = node.material.ks;
		}
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;

		location = params.m_shader->getUniformLocation("material.shininess");
		float shininess;
		if (node.isSelected) {
			shininess = 0.0f;
		} else {
			shininess = node.material.shininess;
		}
		glUniform1f(location, shininess);
		CHECK_GL_ERRORS;
	}

	params.m_shader->disable();
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
	RenderParams params,
	glm::mat4 stackedTrans)
{
	glm::mat4 originalTrans = trans;
	// trans = trans * stackedTrans;
	updateShaderUniforms(params, *this, stackedTrans);

	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	BatchInfo batchInfo = params.m_batchInfoMap->at(meshId);

	//-- Now render the mesh:
	params.m_shader->enable();
	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	params.m_shader->disable();

	for (SceneNode * node : children) {
		// node->renderRecur(shader, view, batchInfoMap, trans * stackedTrans);
		node->renderRecur(params, stackedTrans * trans);
	}
}
