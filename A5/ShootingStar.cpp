#include "ShootingStar.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.hpp>

void Planet::init() {
	m_node->scale(glm::vec3(radius, radius, radius));
	if (m_parent != nullptr) {
		m_node->translate(glm::vec3(orbitR + m_parent->orbitR, 0.0f, 0.0f));
	} else {
		m_node->translate(glm::vec3(orbitR, 0.0f, 0.0f));
	}
	loadTexture();
}

void Planet::update() {
	rotation = glm::mod(rotation + FACTOR * wRot, 360.0f);
	spin = glm::mod(spin + FACTOR * wSpin, 360.0f);
	m_node->set_transform(glm::mat4(1.0f));
	m_node->scale(glm::vec3(radius, radius, radius));
	m_node->rotateAtBeginning('y', spin);
	glm::vec3 location(orbitR * glm::cos(rotation), 0.0f, -orbitR * glm::sin(rotation));
	if (m_parent != nullptr) {
		location += glm::vec3(
			m_parent->orbitR * glm::cos(m_parent->rotation), 0.0f,
			-m_parent->orbitR * glm::sin(m_parent->rotation));
	}
	m_node->translate(location);
}

void Planet::loadTexture()
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
		string texturePath = string("planets/") + m_name + string(".jpg");
    unsigned char *data = stbi_load(CS488Window::getAssetFilePath(texturePath.c_str()).c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				stbi_image_free(data);
    }
    else
    {
        std::cout << "Cubemap texture failed to load at path: " << texturePath << std::endl;
        stbi_image_free(data);
    }
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // stbi_set_flip_vertically_on_load(false);
		m_node->textureId = m_texture;
}

void ShootingStar::init() {
	m_node->scale(glm::vec3(radius, radius, radius));
	m_node->translate(location);
	loadTexture();
}

void ShootingStar::loadTexture()
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
		string texturePath = string("planets/") + m_name + string(".jpg");
    unsigned char *data = stbi_load(CS488Window::getAssetFilePath(texturePath.c_str()).c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				stbi_image_free(data);
    }
    else
    {
        std::cout << "Cubemap texture failed to load at path: " << texturePath << std::endl;
        stbi_image_free(data);
    }
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // stbi_set_flip_vertically_on_load(false);
		m_node->textureId = m_texture;
}

void ShootingStar::update() {
	location += 0.001f * speed;
	m_node->set_transform(glm::mat4(1.0f));
	m_node->scale(glm::vec3(radius, radius, radius));
	m_node->translate(location);
}
