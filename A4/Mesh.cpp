// Termm--Fall 2020

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

using namespace glm;

bool hitSquare(HitParams& params, glm::vec3 m_pos, glm::vec3 normal, glm::vec3 m_size);

Mesh::Mesh( const std::string& fname )
	: Primitive(false)
	, m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}

	for (uint i=0; i<m_vertices.size(); i++) {
		if (i == 0) {
			m_bounding_box_pos = m_vertices[i];
			m_bounding_box_size = m_vertices[i];
		} else {
			m_bounding_box_pos = glm::min(m_bounding_box_pos, m_vertices[i]);
			m_bounding_box_size = glm::max(m_bounding_box_size, m_vertices[i]);
		}
	}
	m_bounding_box_size -= m_bounding_box_pos;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*

  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

bool Mesh::hit(HitParams& params) {
	HitParams paramsBB = params;
	bool hit = hitBoundingBox(paramsBB);

#ifdef RENDER_BOUNDING_VOLUMES
	// printf("Render bounding box as well!\n");
	params = paramsBB;
#else
	if (!hit) {
		return false;
	}

	hit = false;
#endif

	for (auto& triangle : m_faces) {
		// printf("1\n");
		hit |= hitTriangle(params, triangle);
	}

	// if (hit) {
	// 	// printf("Mesh Hit\n");
	// }
	return hit;
}

bool Mesh::hitTriangle(
	HitParams& params,
	const struct Triangle& inTriangle
) {
		const float EPSILON = 1e-7;
    vec3 v1 = m_vertices[inTriangle.v1];
    vec3 v2 = m_vertices[inTriangle.v2];
    vec3 v3 = m_vertices[inTriangle.v3];
    vec3 e1, e2, h, s, q;
    float a,f,u,v;
    e1 = v2 - v1;
    e2 = v3 - v1;
    h = cross(params.rayDirection, e2);
    a = dot(e1, h);
		// printf("v1: %s, v2: %s, v3: %s, h: %s, a: %.4f\n",
		// 	to_string(v1).c_str(), to_string(v2).c_str(), to_string(v3).c_str(),
		// 	to_string(h).c_str(), a);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0f / a;
    s = params.rayOrigin - v1;
    u = f * dot(s, h);
    if (u < 0.0f || u > 1.0f)
        return false;
    q = cross(s, e1);
    v = f * dot(params.rayDirection, q);
    if (v < 0.0f || u + v > 1.0f)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dot(e2, q);

    if (t > EPSILON && t >= params.t_lower && t <= params.t_upper && t < params.t) // ray intersection
    {
        params.t = t;
				params.normal = normalize(cross(e1, e2));
				params.hitPoint = params.rayOrigin + t * params.rayDirection;
				// printf("t %.4f, intersection %s, normal %s\n", t,
				// 	to_string(params.rayOrigin + t * params.rayDirection).c_str(),
				// 	to_string(normalize(cross(e1, e2))).c_str());
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

bool Mesh::hitBoundingBox(HitParams& params) {
  bool hit = false;
  hit |= hitSquare(params, glm::vec3(m_bounding_box_pos), glm::vec3(0.0f, 0.0f, -1.0f), m_bounding_box_size);
  hit |= hitSquare(params, glm::vec3(m_bounding_box_pos.x,
		m_bounding_box_pos.y,
		m_bounding_box_pos.z+m_bounding_box_size.z), glm::vec3(0.0f, 0.0f, 1.0f), m_bounding_box_size);
  hit |= hitSquare(params, glm::vec3(m_bounding_box_pos), glm::vec3(0.0f, -1.0f, 0.0f), m_bounding_box_size);
  hit |= hitSquare(params, glm::vec3(m_bounding_box_pos.x,
		m_bounding_box_pos.y+m_bounding_box_size.y,
		m_bounding_box_pos.z), glm::vec3(0.0f, 1.0f, 0.0f), m_bounding_box_size);
  hit |= hitSquare(params, glm::vec3(m_bounding_box_pos), glm::vec3(-1.0f, 0.0f, 0.0f), m_bounding_box_size);
  hit |= hitSquare(params, glm::vec3(m_bounding_box_pos.x+m_bounding_box_size.x,
		m_bounding_box_pos.y,
		m_bounding_box_pos.z), glm::vec3(1.0f, 0.0f, 0.0f), m_bounding_box_size);
  return hit;
}

bool hitFrame(glm::vec3 relativeCoor, glm::vec3 size) {
	const float propFrame = 0.03;
	glm::vec3 innerPos = size * propFrame;
	relativeCoor -= innerPos;
	size = size * (1 - 2 * propFrame);

	bool hitInner =
    all(greaterThanEqual(relativeCoor, vec3(0.0f))) &&
    all(lessThanEqual(relativeCoor, size));

	return !hitInner;
}

bool hitSquare(HitParams& params, glm::vec3 m_pos, glm::vec3 normal, glm::vec3 m_size) {
  float t = dot(m_pos - params.rayOrigin, normal) / dot(params.rayDirection, normal);
  vec3 intersection = params.rayOrigin + t * params.rayDirection;

  vec3 indicator = vec3(1.0f) - normal * normal;
  vec3 relativeCoor = intersection * indicator - m_pos * indicator;
	vec3 size = m_size * indicator;
  bool hit =
    all(greaterThanEqual(relativeCoor, vec3(0.0f))) &&
    all(lessThanEqual(relativeCoor, size)) &&
    t >= params.t_lower &&
    t <= params.t_upper;

#ifdef RENDER_BOUNDING_VOLUMES
	hit = hit && t < params.t && hitFrame(relativeCoor, size);

	if (hit) {
		params.t = t;
		params.normal = normal;
		params.hitPoint = params.rayOrigin + t * params.rayDirection;
	}
#endif

	return hit;
}
