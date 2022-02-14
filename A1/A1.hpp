// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

private:
	void initGrid();
	void initCubes();
	void initSphere();
	void updatePersistenceLogic();
	void reset();

	// float* generateCubeVerts(vec3 origin, float len);
	float* generateCubeVerts();
	float* generateSphereVerts();
	float distanceToAngle(float dist);
	// Translate spherical coordinates to Cartesian coordinates
	// And fill the result into the destination
	void toCartesian(float r, float alpha, float phi, float* dest);

	// Fields related to the maze Model
	Maze m_maze;

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object


	// Fields related to maze elements geometry (cubes, avatars)
	GLuint m_maze_vao; // Vertex Array Object
	GLuint m_cube_vbo; // Vertex Buffer Object
	size_t m_cube_height;

	GLuint m_sphere_vao; // Vertex Array Object
	GLuint m_sphere_vbo; // Vertex Buffer Object
	size_t m_num_sector;
	size_t m_num_stack;



	// Fields related to colour control
	glm::vec3 m_bg_colour;
	glm::vec3 m_grid_colour;
	glm::vec3 m_cube_colour;
	glm::vec3 m_avatar_colour;
	int current_col;

	// Fields related to rotation control
	bool m_is_pressing_mouse;
	bool m_persisting;
	glm::vec2 m_current_mouse_loc;
	glm::vec2 m_last_mouse_loc; // Location of last mouse clcicked
	float m_h_angle; // The horizontal angle rotated

	// The number of frames that the mouse is clicked and position is held, used in
	// triggering persistence
	unsigned int m_frames_mouse_hold;
	// The speed of persistence, determined by the mouse release speed
	float m_pers_rotation_rate;

	// Fields related to scaling
	float m_scale_factor; // The factor of scaling


	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

};
