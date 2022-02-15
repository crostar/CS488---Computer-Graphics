// Termm--Fall 2020

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>
#include <unordered_set>
#include <vector>

static const bool DEBUG_A2 = false;

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};

class ModelTrans;
class ViewTrans;
class HomoTrans;
class ViewportTrans;
class ClipTrans;
class ProjTrans;
class Controller;

// Class representing a colored object model
class ColoredObject {
public:
	ColoredObject(size_t size) :
		colors(size, glm::vec3()),
		lines(size, std::pair<glm::vec4, glm::vec4>()),
		modelFrame(1.0f) {}

	virtual void accept(ModelTrans* t);
	void accept(ViewTrans* t);
	void accept(HomoTrans* t);
	void accept(ViewportTrans* t);
	void accept(ClipTrans* t);
	void accept(ProjTrans* t);

	void print();
	virtual void reset() = 0;

	std::vector<glm::vec3> colors;
	std::vector<std::pair<glm::vec4, glm::vec4>> lines;
	glm::mat4 modelFrame;
};

// Class representing a cube model
class Cube : public ColoredObject {
public:
	Cube() : ColoredObject(12) {
		reset();
	}

	virtual void reset();
};

// Class representing a world coordinate model
class ModelGnomon : public ColoredObject {
public:
	ModelGnomon() : ColoredObject(3) {
		reset();
	};

	virtual void accept(ModelTrans* t);
	virtual void reset();
};

// Class representing a view coordinate model
class WorldGnomon : public ColoredObject {
public:
	WorldGnomon() : ColoredObject(3) {
		reset();
	};

	virtual void accept(ModelTrans* t);
	virtual void reset();
};

// Class representing a view coordinate model
class ViewportEdge : public ColoredObject {
public:
	ViewportEdge(Controller* controller) : ColoredObject(4), m_controller(controller) {
		reset();
	};

	virtual void reset();

	Controller* m_controller;
};

// Class representing model transformation in the pipeline
class ModelTrans {
public:
	ModelTrans(Controller* controller) : m_controller(controller) {}

	void visit(ColoredObject* obj);
	void visit(ModelGnomon* obj);
	void visit(WorldGnomon* obj);

	glm::mat4 scaler();
	glm::mat4 rotater();
	glm::mat4 translater();

	Controller* m_controller;
};

// Class representing view transformation in the pipeline
class ViewTrans {
public:
	ViewTrans(Controller* controller) : m_controller(controller) {}

	void visit(ColoredObject* obj);

	glm::mat4 view();

	Controller* m_controller;
};

// Class representing projection
class ProjTrans {
public:
	ProjTrans(Controller* controller) : m_controller(controller) {}

	void visit(ColoredObject* obj);

	glm::mat4 projector();

	Controller* m_controller;
};

// Class representing homogenization
class HomoTrans {
public:
	void visit(ColoredObject* obj);
};

// Class representing normalization
class ViewportTrans {
public:
	ViewportTrans(Controller* controller) : m_controller(controller) {}
	void visit(ColoredObject* obj);

	Controller* m_controller;
};

// Class representing clipping
class ClipTrans {
public:
	ClipTrans(Controller* controller) : m_controller(controller) {}
	virtual void visit(ColoredObject* obj) = 0;


	Controller* m_controller;
};

class Clip3DTrans : public ClipTrans {
public:
	Clip3DTrans(Controller* controller) : ClipTrans(controller) {}

	virtual void visit(ColoredObject* obj);
	// perform 3D clipping in place
	bool clip(glm::vec4 &p1, glm::vec4 &p2, glm::vec3 P, glm::vec3 n);
};

class Clip2DTrans : public ClipTrans {
public:
	Clip2DTrans(Controller* controller) : ClipTrans(controller) {}

	virtual void visit(ColoredObject* obj);
	// perform 2D clipping in place
	bool clip(glm::vec2 &p1, glm::vec2 &p2, glm::vec2 P, glm::vec2 n);
};

// Class representing the user-controlled parameters like rotation angle,
// translation distance, etc.
class Controller {
public:
	enum class Mode {
		NONE = 0,
		ROTATE_VIEW,
		TRANSLATE_VIEW,
		PERSPECTIVE,
		ROTATE_MODEL,
		TRANSLATE_MODEL,
		SCALE_MODEL,
		VIEWPORT
	};

	enum class MouseButton {
		NONE = 0,
		LEFT,
		MIDDLE,
		RIGHT
	};

public:
	Controller();

	void updateUponMouseEvent(glm::vec2 mouseLocChange);
	void print();
	void reset();
	void resetViewportAttr();
	void resetModelAttr();
	void resetViewAttr();
	bool pressed(MouseButton m);


	int mode;

	glm::mat4 modelFrame; // Last model frame matrix
	glm::vec3 modelScaler; // scaler factor in x,y,z directions
	glm::vec3 modelRotateAngle; // rotation angle around x,y,z axis
	glm::vec3 modelTranslater; // translate distance in x,y,z directions

	glm::vec3 viewRotateAngle; // rotation angle around x,y,z axis
	glm::vec3 viewTranslater; // translate distance in x,y,z directions

	glm::vec3 lookFrom; // location of camera
	glm::vec3 lookAt; // direction of camera
	glm::vec3 up; // up direction
	float n; // near view plane
	float f; // far view plane
	float aspectRatio; // acpect ration (w/h)
	float fovy; // field of view (theta) in radians

	glm::vec2 viewportLeftBot; // the left-top corner coordinate of the view port
	float viewportWidth; // Width of viewport, not larger than 2
	float viewportHeight; // Height of viewport, not larger that 2

	std::unordered_set<MouseButton> mouseButtonPressed; // current mouse button pressed
	glm::vec2 lastMouseLoc; // location of the mouse in the last frame
	// TODO: more params...
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

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

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);
	void drawObject(ColoredObject* obj);
	std::pair<glm::vec2, glm::vec2> to2DLine(std::pair<glm::vec4, glm::vec4> l);
	void clearFrames();

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	std::unique_ptr<Controller> m_controller;

	std::vector<std::unique_ptr<ColoredObject>> m_objects;
	std::unique_ptr<ColoredObject> m_vp_edge;

	std::unique_ptr<ModelTrans> m_model_trans;
	std::unique_ptr<ViewTrans> m_view_trans;
	std::unique_ptr<ProjTrans> m_proj_trans;
	std::unique_ptr<ClipTrans> m_clip_3d_trans;
	std::unique_ptr<ClipTrans> m_clip_2d_trans;
	std::unique_ptr<HomoTrans> m_homo_trans;
	std::unique_ptr<ViewportTrans> m_vp_trans;
};
