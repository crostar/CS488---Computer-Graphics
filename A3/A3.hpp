// Termm-Fall 2020

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "TrackBall.hpp"

#include <glm/glm.hpp>
#include <memory>

#define DEBUG_A3 1

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

class Controller {
public:
	enum class Mode {
		NONE = 0,
		POSITION = 1,
		JOINTS = 2
	};

	enum class MouseButton {
		NONE = 0,
		LEFT,
		MIDDLE,
		RIGHT
	};

public:
	Controller();

	void updateUponMouseMoveEvent(glm::vec2 mouseLocChange, float fDiameter, vec2 center);
	void updateUponMouseInputEvent();
	void print();
	void reset();
	bool pressed(MouseButton m);
	vec2 pixelToFC(vec2 loc, vec2 center);

	int mode;
	bool picking;

	// glm::mat4 modelFrame; // Last model frame matrix
	// glm::vec3 modelScaler; // scaler factor in x,y,z directions
	// glm::vec3 modelRotateAngle; // rotation angle around x,y,z axis
	glm::vec3 modelTranslater; // translate distance in x,y,z directions

	// glm::vec3 viewRotateAngle; // rotation angle around x,y,z axis
	// glm::vec3 viewTranslater; // translate distance in x,y,z directions

	MouseButton mouseButtonPressed; // current mouse button pressed
	glm::vec2 lastMouseLoc; // location of the mouse in the last frame
	std::unique_ptr<TrackBall> m_trackBall;
};

class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

	void initGrid();
	GLuint m_grid_vao;
	GLuint m_grid_vbo;

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	// void renderSceneGraph(const SceneNode &node);
	void renderSceneGraph(SceneNode &node);
	void renderArcCircle();
	void updateModel();


	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;

	std::unique_ptr<Controller> m_controller;
};
