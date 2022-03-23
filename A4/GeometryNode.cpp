// Termm--Fall 2020

#include "GeometryNode.hpp"

using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

bool GeometryNode::hit(HitParams& params) {
	params.rayOrigin = vec3(get_inverse() * vec4(params.rayOrigin, 1.0f));
	params.rayDirection = vec3(get_inverse() * vec4(params.rayDirection, 0.0f));

	bool hit = m_primitive->hit(params);
	if (hit) {
		params.objNode = this;

		// params.hitPoint = vec3(get_transform() * vec4(params.hitPoint, 1.0f));
		// // params.normal = vec3(transpose(get_inverse()) * vec4(params.normal, 0.0f));
		// params.normal = normalize(vec3(transpose(get_inverse()) * vec4(params.normal, 0.0f)));
	}

	for (auto child : children) {
		hit |= child->hit(params);
	}

	if (hit) {
		params.hitPoint = vec3(get_transform() * vec4(params.hitPoint, 1.0f));
		params.normal = vec3(transpose(get_inverse()) * vec4(params.normal, 0.0f));
		// params.normal = normalize(vec3(transpose(get_inverse()) * vec4(params.normal, 0.0f)));
	}
	// params.hitPoint = vec3(get_transform() * vec4(params.hitPoint, 1.0f));
	// if (get_inverse() != mat4(1.0f)) {
	// 	printf("Before: %s, after: %s\n", to_string(get_inverse()).c_str(),
	// 	 	to_string(glm::transpose(glm::mat3(get_inverse()))).c_str());
	// }
	// params.normal = transpose(mat3(get_inverse())) * params.normal;
	// params.normal = normalize(vec3(transpose(get_inverse()) * vec4(params.normal, 0.0f)));

	params.rayOrigin = vec3(get_transform() * vec4(params.rayOrigin, 1.0f));
	params.rayDirection = vec3(get_transform() * vec4(params.rayDirection, 0.0f));

	return hit;
}


void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}
