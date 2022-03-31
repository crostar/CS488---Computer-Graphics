#pragma once

#include <memory>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"

#include <glm/glm.hpp>

class Planet {
public:
	Planet(const char* name, GeometryNode* node, float r, float oR, float wrt, float wsp) :
		m_name(name), m_node(node), m_parent(nullptr),
		radius(r), orbitR(oR),
		rotation(0.0f), spin(0.0f),
		wRot(wrt), wSpin(wsp), m_texture(0) {}

	void init();
	void update();
	void loadTexture();

	static constexpr float FACTOR = 0.5;

	std::string m_name;
	GeometryNode* m_node;
	// Only direct hierachy allowed i.e. Earth->Moon
	struct Planet* m_parent;

	float radius;
	float orbitR;
	float rotation;
	float spin;
	float wRot;
	float wSpin;

	uint m_texture;
};

class ShootingStar {
public:
	ShootingStar(const char* name, GeometryNode* node, float r, const glm::vec3& loc,
		const glm::vec3& sp) :
		m_name(name), m_node(node), radius(r), location(loc), speed(sp), m_texture(0) {}
	void init();
	void update();
	void loadTexture();

	// static constexpr float SFACTOR = 0.5;

	std::string m_name;
	GeometryNode* m_node;
	float radius;
	glm::vec3 location;
	glm::vec3 speed;

	uint m_texture;
};
