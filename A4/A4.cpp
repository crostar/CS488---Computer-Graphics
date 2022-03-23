// Termm--Fall 2020

#include <glm/ext.hpp>

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "HitParams.hpp"
#include <cstdlib>

glm::vec3 generateBGColor(float xf, float yf, size_t w, size_t h);


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
) {
	// pixelToWorldCoor()
	// for each ray:
	//   getIntersect
	//   getRGB
  // Fill in raytracing code here...
	Image background;
	background.readPNG("background.png");
	background.compress(image.width(), image.height());
	background.savePng("testBG.png");

  std::cout << "F20: Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();

#ifdef ENABLE_ANTI_ALIASING
	const size_t JITTERING_SIZE = 5;
#else
	const size_t JITTERING_SIZE = 1;
#endif

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			glm::vec3 origin(eye);
			glm::vec3 direction;
			glm::vec3 color(0.0f);
			bool hit = false;
			for (uint q=0; q<JITTERING_SIZE; q++) {
				for (uint p=0; p<JITTERING_SIZE; p++) {
					float xf = x + (p + rand() / double(RAND_MAX)) / JITTERING_SIZE;
					float yf = y + (q + rand() / double(RAND_MAX)) / JITTERING_SIZE;
					// Pass h-y in to set (0,0) to the bottom-left corner
					getDirection(eye, view, up, fovy, w, h, xf, h-yf, direction);
					hit = hit || getRGB(root, origin, direction, ambient, lights, color);
				}
			}
			color = color;// / (JITTERING_SIZE * JITTERING_SIZE);

			// if (true) {
			if (hit) {
				// color = glm::vec3(0.3f);
				// Red:
				image(x, y, 0) = (double) color.x;
				// Green:
				image(x, y, 1) = (double) color.y;
				// Blue:
				image(x, y, 2) = (double) color.z;
			} else {
				// color = glm::vec3(0.3f);
				// Red:
				image(x, y, 0) = background(x, y, 0);
				// Green:
				image(x, y, 1) = background(x, y, 1);
				// Blue:
				image(x, y, 2) = background(x, y, 2);
			}


		}
		std::cout << "Rendering " << y << " / " << h << std::endl;
	}

}

void getDirection(
	glm::vec3 eye, glm::vec3 view, glm::vec3 up,
	double fovy, size_t w, size_t h, float x, float y,
	glm::vec3& direction) {
		float wf = static_cast<float>(w), hf = static_cast<float>(h);
		float d = hf / (2 * glm::tan(glm::radians(fovy) / 2.0f));
		// Step 1
		glm::mat4 T1 = glm::translate(glm::mat4(), glm::vec3(-wf/2.0f, -hf/2.0f, d));
		glm::mat4 S2 = glm::scale(glm::mat4(), glm::vec3(-1.0f, 1.0f, 1.0f));

		glm::vec3 w_ = glm::normalize(view - eye);
		glm::vec3 u_ = glm::normalize(glm::cross(up, w_));
		glm::vec3 v_ = glm::normalize(glm::cross(w_, u_));
		glm::mat4 R3 = glm::transpose ( glm::mat4(
			u_.x, v_.x, w_.x, 0.0f,
			u_.y, v_.y, w_.y, 0.0f,
			u_.z, v_.z, w_.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		glm::mat4 T4 = glm::translate(glm::mat4(), glm::vec3(eye.x, eye.y, eye.z));
		glm::vec4 pixelCoor(x, y, 0.0f, 1.0f);
		glm::vec4 worldCoor = T4 * R3 * S2 * T1 * pixelCoor;
		direction = (glm::vec3(worldCoor) - eye);
}

// Assumption: root node is a SceneNode
bool getRGB(
	SceneNode* root, glm::vec3 rayOrigin, glm::vec3 rayDirection,
	// Lighting parameters
	const glm::vec3 & ambient,
	const std::list<Light *> & lights,
	glm::vec3& colorAcc,
	bool debug
) {
	HitParams params(rayOrigin, rayDirection, 20.0f, 0.0f);
	bool hit = root->hit(params);
	if (debug) {
		std::cout << "Hit: " << hit << std::endl;
		std::cout << params << std::endl;
	}

	if (!hit) {
		// printf("No hit\n");
		return false;
	}

	GeometryNode* hitNode = params.objNode;
	// glm::vec3 normal = normalize(params.normal);
	glm::vec3 normal = params.normal;
	glm::vec3 hitPoint = params.hitPoint;
	// hitPoint = params.t * rayDirection + rayOrigin;

	glm::vec3 color(0.0f);
	color = hitNode->m_material->getKd() * ambient;

	for(const Light * light : lights) {
		glm::vec3 lightDirection = glm::normalize(light->position - hitPoint);
		glm::vec3 viewDirection = - glm::normalize(rayDirection);
		glm::vec3 reverseLightDirection = 2 * glm::dot(
			lightDirection, normal
		) * normal - lightDirection;


		const float epsilon = 1e-5;
		HitParams shadowParams(hitPoint, light->position - hitPoint, 5.0f, epsilon);
		bool shadow = root->hit(shadowParams);
		// if (debug) {
		// 	// std::cout << "Shadow: " << shadow << std::endl;
		// 	std::cout << shadowParams << std::endl;
		// }
		// std::cout << "Blocked " << shadow << std::endl;
		// bool shadow = false;
		if (!shadow) {
			// diffuse
			color +=
				hitNode->m_material->getKd() *
				glm::max(0.0f, glm::dot(lightDirection, normal)) *
				light->colour;

			// specular
			color += hitNode->m_material->getKs() * glm::pow(
				glm::max(0.0f, glm::dot(reverseLightDirection, viewDirection)), hitNode->m_material->getShininess()
			) * light->colour;
		}
	}

	// std::cout << "Hit found at t= " << t << std::endl;
	colorAcc += color;
	return true;
}

glm::vec3 generateBGColor(float xf, float yf, size_t w, size_t h) {
	glm::vec3 darkBlue(0.1f, 0.4f, 0.9f);
	glm::vec3 lightBlue(0.45f, 0.95f, 0.8f);
	glm::vec3 delta = darkBlue - lightBlue;
	float prop = yf / h;
	return lightBlue + prop * delta;
}
