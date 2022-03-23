// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"



void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
);

void getDirection(
	glm::vec3 eye, glm::vec3 view, glm::vec3 up,
	double fovy, size_t w, size_t h, float x, float y,
	glm::vec3& direction);

bool getRGB(
	SceneNode* root, glm::vec3 rayOrigin, glm::vec3 rayDirection,
	// Lighting parameters
	const glm::vec3 & ambient,
	const std::list<Light *> & lights,
	glm::vec3& colorAcc,
	bool debug = false
);
