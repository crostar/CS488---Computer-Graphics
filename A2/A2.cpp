// Termm--Fall 2020

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

const float PI = 3.14159265f;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f)),
		m_controller(new Controller()),
		m_vp_edge(new ViewportEdge(m_controller.get())),
		m_model_trans(new ModelTrans(m_controller.get())),
		m_view_trans(new ViewTrans(m_controller.get())),
		m_proj_trans(new ProjTrans(m_controller.get())),
		m_clip_3d_trans(new Clip3DTrans(m_controller.get())),
		m_clip_2d_trans(new Clip2DTrans(m_controller.get())),
		m_homo_trans(new HomoTrans()),
		m_vp_trans(new ViewportTrans(m_controller.get()))
{
	m_objects.emplace_back(new Cube());
	m_objects.emplace_back(new ModelGnomon());
	m_objects.emplace_back(new WorldGnomon());
}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Spring 2020
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const vec2 & V0,   // Line Start (NDC coordinate)
		const vec2 & V1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//---------------------------------------------------------------------------------------
std::pair<vec2, vec2> A2::to2DLine(std::pair<vec4, vec4> l) {
	return std::make_pair(vec2(l.first), vec2(l.second));
}

void A2::drawObject(ColoredObject* obj) {
	for (int i=0; i<obj->lines.size(); i++) {
		setLineColour(obj->colors[i]);
		auto line = to2DLine(obj->lines[i]);
		drawLine(line.first, line.second);
	}
	//
	// if (DEBUG_A2) {
	// 	for (int i=0; i<obj->lines.size(); i++) {
	// 		auto line = to2DLine(obj->lines[i]);
	// 		cout << "( " << line.first << ", " << line.second << ") ";
	// 	}
	// 	cout << endl;
	// }
}


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...
	if (DEBUG_A2) {
		cout << "New Frame " << endl;
	}
	// Call at the beginning of frame, before drawing lines:
	initLineData();

	for (auto &obj : m_objects) {
		obj->reset();
		obj->accept(m_model_trans.get());
		obj->accept(m_view_trans.get());
		obj->accept(m_clip_3d_trans.get());
		obj->accept(m_proj_trans.get());
		obj->accept(m_homo_trans.get());
		obj->accept(m_clip_2d_trans.get());
		obj->accept(m_vp_trans.get());
		// if (DEBUG_A2) {
		// 	obj->print();
		// }
		drawObject(obj.get());
	}

	m_vp_edge->reset();
	drawObject(m_vp_edge.get());

	m_controller->resetModelAttr();
	m_controller->resetViewAttr();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset" ) ) {
			m_controller->reset();
			clearFrames();
		}

		ImGui::RadioButton( "ROTATE VIEW (o)", &(m_controller->mode), 1 );
		ImGui::RadioButton( "TRANSLATE_VIEW (e)", &(m_controller->mode), 2 );
		ImGui::RadioButton( "PERSPECTIVE (p)", &(m_controller->mode), 3 );
		ImGui::RadioButton( "ROTATE_MODEL (r)", &(m_controller->mode), 4 );
		ImGui::RadioButton( "TRANSLATE_MODEL (t)", &(m_controller->mode), 5 );
		ImGui::RadioButton( "SCALE_MODEL (s)", &(m_controller->mode), 6 );
		ImGui::RadioButton( "VIEWPORT (v)", &(m_controller->mode), 7 );

		ImGui::Text( "Camera status: near(%.1f), far(%.1f), fov(%.1f degrees)",
			m_controller->n, m_controller->f, glm::degrees(m_controller->fovy));

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::clearFrames() {
	for (auto &obj : m_objects) {
		obj->modelFrame = mat4(1.0f);
	}
	m_view_trans->viewFrame = m_view_trans->baseView();
}
//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	vec2 curMouseLoc (
			(2.0f * xPos) / m_windowWidth - 1.0f,
			1.0f - ( (2.0f * yPos) / m_windowHeight)
	);

	vec2 mouseLocChange = curMouseLoc - m_controller->lastMouseLoc;
	m_controller->lastMouseLoc = curMouseLoc;

	m_controller->updateUponMouseEvent(mouseLocChange);

	eventHandled = true;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (actions == GLFW_PRESS) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				m_controller->mouseButtonPressed.insert(Controller::MouseButton::LEFT);
				if (m_controller->mode == (int) Controller::Mode::VIEWPORT) {
					m_controller->resetViewportAttr();
				}
			} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				m_controller->mouseButtonPressed.insert(Controller::MouseButton::RIGHT);
			} else if (button = GLFW_MOUSE_BUTTON_MIDDLE) {
				m_controller->mouseButtonPressed.insert(Controller::MouseButton::MIDDLE);
			}
		} else if (actions == GLFW_RELEASE) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				m_controller->mouseButtonPressed.erase(Controller::MouseButton::LEFT);
			} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				m_controller->mouseButtonPressed.erase(Controller::MouseButton::RIGHT);
			} else if (button = GLFW_MOUSE_BUTTON_MIDDLE) {
				m_controller->mouseButtonPressed.erase(Controller::MouseButton::MIDDLE);
			}
		}
	}

	eventHandled = true;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (action == GLFW_PRESS) {
		// Create Button, and check if it was clicked:
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		if (key == GLFW_KEY_A) {
			m_controller->reset();
			clearFrames();
		}

		if (key == GLFW_KEY_O) {
			m_controller->mode = (int) Controller::Mode::ROTATE_VIEW;
		}

		if (key == GLFW_KEY_E) {
			m_controller->mode = (int) Controller::Mode::TRANSLATE_VIEW;
		}

		if (key == GLFW_KEY_P) {
			m_controller->mode = (int) Controller::Mode::PERSPECTIVE;
		}

		if (key == GLFW_KEY_R) {
			m_controller->mode = (int) Controller::Mode::ROTATE_MODEL;
		}

		if (key == GLFW_KEY_T) {
			m_controller->mode = (int) Controller::Mode::TRANSLATE_MODEL;
		}

		if (key == GLFW_KEY_S) {
			m_controller->mode = (int) Controller::Mode::SCALE_MODEL;
		}

		if (key == GLFW_KEY_V) {
			m_controller->mode = (int) Controller::Mode::VIEWPORT;
		}
	}

	eventHandled = true;

	return eventHandled;
}


//----------------------------------------------------------------------------------------
// Definition for model class ------------------------------------------------------------
//----------------------------------------------------------------------------------------
void ColoredObject::accept(ModelTrans* t) {
	t->visit(this);
}

void ModelGnomon::accept(ModelTrans* t) {
	t->visit(this);
}

void WorldGnomon::accept(ModelTrans* t) {
	t->visit(this);
}

void ColoredObject::accept(ViewTrans* t) {
	t->visit(this);
}

void ColoredObject::accept(HomoTrans* t) {
	t->visit(this);
}

void ColoredObject::accept(ViewportTrans* t) {
	t->visit(this);
}

void ColoredObject::accept(ClipTrans* t) {
	t->visit(this);
}

void ColoredObject::accept(ProjTrans *t) {
	t->visit(this);
}

void Cube::reset() {
	lines[0] = std::make_pair(vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, -1.0f, 1.0f));
	lines[1] = std::make_pair(vec4(1.0f, 1.0f, -1.0f, 1.0f), vec4(-1.0f, 1.0f, -1.0f, 1.0f));
	lines[2] = std::make_pair(vec4(-1.0f, 1.0f, -1.0f, 1.0f), vec4(-1.0f, 1.0f, 1.0f, 1.0f));
	lines[3] = std::make_pair(vec4(-1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	lines[4] = std::make_pair(vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(1.0f, -1.0f, 1.0f, 1.0f));
	lines[5] = std::make_pair(vec4(1.0f, 1.0f, -1.0f, 1.0f), vec4(1.0f, -1.0f, -1.0f, 1.0f));
	lines[6] = std::make_pair(vec4(-1.0f, 1.0f, -1.0f, 1.0f), vec4(-1.0f, -1.0f, -1.0f, 1.0f));
	lines[7] = std::make_pair(vec4(-1.0f, 1.0f, 1.0f, 1.0f), vec4(-1.0f, -1.0f, 1.0f, 1.0f));
	lines[8] = std::make_pair(vec4(1.0f, -1.0f, 1.0f, 1.0f), vec4(1.0f, -1.0f, -1.0f, 1.0f));
	lines[9] = std::make_pair(vec4(1.0f, -1.0f, -1.0f, 1.0f), vec4(-1.0f, -1.0f, -1.0f, 1.0f));
	lines[10] = std::make_pair(vec4(-1.0f, -1.0f, -1.0f, 1.0f), vec4(-1.0f, -1.0f, 1.0f, 1.0f));
	lines[11] = std::make_pair(vec4(-1.0f, -1.0f, 1.0f, 1.0f), vec4(1.0f, -1.0f, 1.0f, 1.0f));
	for (int i=0; i<12; i++) {
		colors[i] = vec3(255.0f, 255.0f, 255.0f);
	}
}

void ModelGnomon::reset() {
	lines[0] = std::make_pair(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.2f, 0.0f, 0.0f, 1.0f));
	lines[1] = std::make_pair(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.2f, 0.0f, 1.0f));
	lines[2] = std::make_pair(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 0.2f, 1.0f));
	colors[0] = vec3(255.0f, 0.0f, 0.0f);
	colors[1] = vec3(0.0f, 255.0f, 0.0f);
	colors[2] = vec3(0.0f, 0.0f, 255.0f);
}

void WorldGnomon::reset() {
	lines[0] = std::make_pair(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.2f, 0.0f, 0.0f, 1.0f));
	lines[1] = std::make_pair(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.2f, 0.0f, 1.0f));
	lines[2] = std::make_pair(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 0.2f, 1.0f));
	colors[0] = vec3(0.0f, 255.0f, 255.0f);
	colors[1] = vec3(255.0f, 0.0f, 255.0f);
	colors[2] = vec3(255.0f, 255.0f, 0.0f);
}

void ViewportEdge::reset() {
		float h = m_controller->viewportHeight, w = m_controller->viewportWidth;
		vec2 lt = m_controller->viewportLeftBot;
		lines[0] = std::make_pair(vec4(lt.x, lt.y, 0.0f, 1.0f), vec4(lt.x+w, lt.y, 0.0f, 1.0f));
		//lines[0] = std::make_pair(vec4(-1.0f, 1.0f, 0.0f, 1.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f));
		lines[1] = std::make_pair(vec4(lt.x+w, lt.y, 0.0f, 1.0f), vec4(lt.x+w, lt.y+h, 0.0f, 1.0f));
		lines[2] = std::make_pair(vec4(lt.x+w, lt.y+h, 0.0f, 1.0f), vec4(lt.x, lt.y+h, 0.0f, 1.0f));
		lines[3] = std::make_pair(vec4(lt.x, lt.y+h, 0.0f, 1.0f), vec4(lt.x, lt.y, 0.0f, 1.0f));
		for (int i=0; i<4; i++) {
			colors[i] = vec3(0.0f, 0.0f, 0.0f);
		}
}

void ColoredObject::print() {
	cout << "OBJ: ";
	for (auto line : lines) {
		cout << "(" << line.first << ", " << line.second << ")" << endl;
	}
}
//----------------------------------------------------------------------------------------
// Definition for transformation class ------------------------------------------------------------
//----------------------------------------------------------------------------------------

// Model Transformations
mat4 ModelTrans::scaler() {
	return glm::transpose(mat4(m_controller->modelScaler.x, 0, 0, 0,
																	 0, m_controller->modelScaler.y, 0, 0,
																 	 0, 0, m_controller->modelScaler.z, 0,
																   0, 0, 0, 1));
}

mat4 ModelTrans::translater() {
	return glm::transpose(mat4(1, 0, 0, m_controller->modelTranslater.x,
																	 0, 1, 0, m_controller->modelTranslater.y,
																   0, 0, 1, m_controller->modelTranslater.z,
																 	 0, 0, 0, 1));
}

// TODO: rotater
mat4 ModelTrans::rotater() {
	float thetax = m_controller->modelRotateAngle.x;
	float thetay = m_controller->modelRotateAngle.y;
	float thetaz = m_controller->modelRotateAngle.z;
	return glm::transpose(
		mat4(cos(thetaz), -sin(thetaz), 0, 0,
							sin(thetaz), cos(thetaz), 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1)
		* mat4(1, 0, 0, 0,
								0, cos(thetax), -sin(thetax), 0,
								0, sin(thetax), cos(thetax), 0,
							  0, 0, 0, 1)
		* mat4(cos(thetay), 0, sin(thetay), 0,
								0, 1, 0, 0,
								-sin(thetay), 0, cos(thetay), 0,
								0, 0, 0, 1)
	);
}

void ModelTrans::visit(ColoredObject* obj) {
	mat4 M = obj->modelFrame * translater() * rotater() * scaler();
	// if (DEBUG_A2) {
	// 	cout << M << endl;
	// }
	for (auto& line : obj->lines) {
		line.first = M * line.first;
		line.second = M * line.second;
	}
	obj->modelFrame = M;
}

void ModelTrans::visit(ModelGnomon* obj) {
	mat4 M = obj->modelFrame * translater() * rotater();
	for (auto& line : obj->lines) {
		line.first = M * line.first;
		line.second = M * line.second;
	}
	obj->modelFrame = M;
}

void ModelTrans::visit(WorldGnomon* obj) {
	return;
}

// View and Projection transformations

// Translation in view relative to the view frame
mat4 ViewTrans::viewT() {
	float thetax = m_controller->viewRotateAngle.x;
	float thetay = m_controller->viewRotateAngle.y;
	float thetaz = m_controller->viewRotateAngle.z;
	return glm::transpose(
		mat4(cos(thetaz), -sin(thetaz), 0, 0,
							sin(thetaz), cos(thetaz), 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1)
		* mat4(1, 0, 0, 0,
								0, cos(thetax), -sin(thetax), 0,
								0, sin(thetax), cos(thetax), 0,
								0, 0, 0, 1)
		* mat4(cos(thetay), 0, sin(thetay), 0,
								0, 1, 0, 0,
								-sin(thetay), 0, cos(thetay), 0,
								0, 0, 0, 1)
	) * glm::transpose(
		mat4(1, 0, 0, m_controller->viewTranslater.x,
				 0, 1, 0, m_controller->viewTranslater.y,
			   0, 0, 1, m_controller->viewTranslater.z,
			 	 0, 0, 0, 1)
	);
}

mat4 ViewTrans::baseView() {
	vec3 viewDirection = m_controller->lookAt - m_controller->lookFrom;
	vec3 vz = - viewDirection / glm::length(viewDirection);
	vec3 vx = glm::cross(m_controller->up, vz);
	vx = vx / glm::length(vx);
	vec3 vy = glm::cross(vz, vx);

	mat4 R = glm::transpose(
		mat4(vec4(vx, 0),
			 		 vec4(vy, 0),
			 	 	 vec4(vz, 0),
			 	 	 vec4(0, 0, 0, 1))
	);
	mat4 T = glm::transpose(
	  mat4(1, 0, 0, -m_controller->lookFrom.x,
				 0, 1, 0, -m_controller->lookFrom.y,
			   0, 0, 1, -m_controller->lookFrom.z,
			 	 0, 0, 0, 1)
	);
	mat4 res = R * T;

	// if (DEBUG_A2) {
	// 	cout << "vz: " << vz << "lookFrom: " << m_controller->lookFrom
	// 			 << "R: " << R << "T:" << T <<"Res" << res << endl;
	// }
	return res;
}

void ViewTrans::visit(ColoredObject* obj) {
	mat4 V = glm::inverse(viewT()) * viewFrame;
	for (auto& line : obj->lines) {
		line.first = V * line.first;
		line.second = V * line.second;
	}
	viewFrame = V;
}

// Projection
mat4 ProjTrans::projector() {
	float theta = m_controller->fovy;
	float aspect = m_controller->aspectRatio;
	float f = m_controller->f;
	float n = m_controller->n;
	return glm::transpose(
		glm::mat4(1 / (tan(theta/2) * aspect), 0, 0, 0,
							0, 1 / tan(theta/2), 0, 0,
							0, 0, (f+n) / (f-n), -2*f*n / (f-n),
							0, 0, -1, 0)
	);
}

void ProjTrans::visit(ColoredObject* obj) {
	mat4 P = projector();
	for (auto& line : obj->lines) {
		line.first = P * line.first;
		line.second = P * line.second;
	}
}

// Homogenization
void HomoTrans::visit(ColoredObject* obj) {
	for (auto& line : obj->lines) {
		line.first = line.first / line.first[3];
		line.second = line.second / line.second[3];
	}
}

// Normalization
void ViewportTrans::visit(ColoredObject* obj) {
	float Lw = 2.0f, Hw = 2.0f;
	float Lv = m_controller->viewportWidth, Hv = m_controller->viewportHeight;
	// Determine the real bottom-left corner
	vec2 lb;
	lb.x = glm::min(m_controller->viewportLeftBot.x + Lv, m_controller->viewportLeftBot.x);
	lb.y = glm::min(m_controller->viewportLeftBot.y + Hv, m_controller->viewportLeftBot.y);
	Lv = std::abs(Lv);
	Hv = std::abs(Hv);
	for (auto& line : obj->lines) {
		line.first.x = Lv / Lw * (line.first.x - (-1.0f)) + lb.x;
		line.first.y = Hv / Hw * (line.first.y - (-1.0f)) + lb.y;
		line.second.x = Lv / Lw * (line.second.x - (-1.0f)) + lb.x;
		line.second.y = Hv / Hw * (line.second.y - (-1.0f)) + lb.y;
	}
}

// Clip in 3D space before projection
void Clip3DTrans::visit(ColoredObject* obj) {
	// Clip to the near plane before projection
	vec3 n = m_controller->lookAt - m_controller->lookFrom;
	n = n / glm::length(n);
	vec3 P = m_controller->lookAt + n * m_controller->n;
	for (auto& line : obj->lines) {
		bool accept = clip(line.first, line.second, P, n);
		// If reject, simply set two points the same so that it cannor form a line
		if (!accept) {
			line.first = line.second;
		}
	}
	// Clip to the far plane before projection
	P = m_controller->lookAt + n * m_controller->f;
	n = -n;
	for (auto& line : obj->lines) {
		bool accept = clip(line.first, line.second, P, n);
		if (!accept) {
			line.first = line.second;
		}
	}
}

void Clip2DTrans::visit(ColoredObject *obj) {
	std::vector<vec2> P{vec2(1.0f, 0.0f), vec2(0.0f, -1.0f), vec2{-1.0f, 0.0f}, vec2(0.0f, 1.0f)};

	for (auto& line : obj->lines) {
		if (DEBUG_A2) {
			cout << "Before 2D clipping: (" << line.first << ", " << line.second << endl;
		}
		// Clip to the left and right (X=1 or -1)
		vec2 xy1(line.first.x, line.first.y);
		vec2 xy2(line.second.x, line.second.y);
		bool accept = true;
		for (auto p : P) {
			vec2 n = vec2(0.0f, 0.0f) - p;
			accept = accept && clip(xy1, xy2, p, n);
		}
		if (!accept) {
			line.first = line.second;
		} else {
			line.first.x = xy1.x; line.first.y = xy1.y;
			line.second.x = xy2.x; line.second.y = xy2.y;
		}

		// if (DEBUG_A2) {
		// 	cout << "After 2D clipping: (" << line.first << ", " << line.second << endl;
		// }
	}
}

// cut line p1p2 using line P, n where P is a point and n is the normal vector
bool Clip3DTrans::clip(vec4 &p1, vec4 &p2, vec3 P, vec3 n) {
	if (DEBUG_A2) {
		cout << "Clipping 3d line (" << p1 << ", " << p2 << ") to the plane P=" << P << ", n=" << n << "  ";
	}
	float wecA = glm::dot((vec3(p1) - P), n);
	float wecB = glm::dot((vec3(p2) - P), n);
	if (wecA < 0 && wecB < 0) {
		if (DEBUG_A2) {
			cout << "Reject" << endl;
		}
		return false;
	} else if (wecA > 0 && wecB > 0) {
		if (DEBUG_A2) {
			cout << "Accept" << endl;
		}
		return true;
	}
	float t = wecA / (wecA - wecB);
	if (wecA < 0) {
		p1 = p1 + t * (p2 - p1);
	} else {
		p2 = p1 + t * (p2 - p1);
	}
	if (DEBUG_A2) {
		cout << "Accept, new line (" << p1 << ", " << p2 << ")" << endl;
	}
	return true;
}

// cut line p1p2 using line P, n where P is a point and n is the normal vector
bool Clip2DTrans::clip(vec2 &p1, vec2 &p2, vec2 P, vec2 n) {
	if (DEBUG_A2) {
		cout << "Clipping 2d line (" << p1 << ", " << p2 << ") to the plane P=" << P << ", n=" << n << "  ";
	}
	float wecA = glm::dot((p1 - P), n);
	float wecB = glm::dot((p2 - P), n);
	if (wecA < 0 && wecB < 0) {
		if (DEBUG_A2) {
			cout << "Reject" << endl;
		}
		return false;
	} else if (wecA > 0 && wecB > 0) {
		if (DEBUG_A2) {
			cout << "Accept" << endl;
		}
		return true;
	}
	float t = wecA / (wecA - wecB);
	if (wecA < 0) {
		p1 = p1 + t * (p2 - p1);
	} else {
		p2 = p1 + t * (p2 - p1);
	}
	if (DEBUG_A2) {
		cout << "Accept, t=" << t << ", new line (" << p1 << ", " << p2 << ")" << endl;
	}
	return true;
}
//----------------------------------------------------------------------------------------
// Definition for controller class ------------------------------------------------------------
//----------------------------------------------------------------------------------------
Controller::Controller()
	: mode(0),
		lastMouseLoc(0.0f, 0.0f)
	{
		reset();
	}

// Parse the mouse input into the controller
void Controller::updateUponMouseEvent(vec2 mouseLocChange) {
	if (mouseButtonPressed.empty()) {
		return;
	}

	if ((Controller::Mode) mode == Controller::Mode::ROTATE_VIEW)
	{
		if (pressed(Controller::MouseButton::LEFT)) {
			viewRotateAngle.x += 2 * PI * mouseLocChange.x;
		}
		if (pressed(Controller::MouseButton::MIDDLE)) {
			viewRotateAngle.y += 2 * PI * mouseLocChange.x;
		}
		if (pressed(Controller::MouseButton::RIGHT)) {
			viewRotateAngle.z += 2 * PI * mouseLocChange.x;
		}
	}
	else if ((Controller::Mode) mode == Controller::Mode::TRANSLATE_VIEW)
	{
		if (pressed(Controller::MouseButton::LEFT)) {
			viewTranslater.x += mouseLocChange.x;
		}
		if (pressed(Controller::MouseButton::MIDDLE)) {
			viewTranslater.y += mouseLocChange.x;
		}
		if (pressed(Controller::MouseButton::RIGHT)) {
			viewTranslater.z += mouseLocChange.x;
		}
	}
	else if ((Controller::Mode) mode == Controller::Mode::PERSPECTIVE)
	{
		if (pressed(Controller::MouseButton::LEFT)) {
			fovy = glm::clamp(fovy + mouseLocChange.x, glm::radians(5.0f), glm::radians(160.0f));
		}
		if (pressed(Controller::MouseButton::MIDDLE)) {
			n = glm::clamp(n + mouseLocChange.x, 0.2f, f);
		}
		if (pressed(Controller::MouseButton::RIGHT)) {
			f = glm::clamp(f + mouseLocChange.x, n, 20.0f);
		}
	}
	else if ((Controller::Mode) mode == Controller::Mode::ROTATE_MODEL)
	{
		if (pressed(Controller::MouseButton::LEFT)) {
			modelRotateAngle.x += 2 * PI * mouseLocChange.x;
		}
		if (pressed(Controller::MouseButton::MIDDLE)) {
			modelRotateAngle.y += 2 * PI * mouseLocChange.x;
		}
		if (pressed(Controller::MouseButton::RIGHT)) {
			modelRotateAngle.z += 2 * PI * mouseLocChange.x;
		}
	}
	else if ((Controller::Mode) mode == Controller::Mode::SCALE_MODEL)
	{
		if (pressed(Controller::MouseButton::LEFT)) {
			modelScaler.x += mouseLocChange.x;
		}
		if (pressed(Controller::MouseButton::MIDDLE)) {
			modelScaler.y += mouseLocChange.x;
		}
		if (pressed(Controller::MouseButton::RIGHT)) {
			modelScaler.z += mouseLocChange.x;
		}
	}
	else if ((Controller::Mode) mode == Controller::Mode::TRANSLATE_MODEL)
	{
		if (pressed(Controller::MouseButton::LEFT)) {
			modelTranslater.x += mouseLocChange.x;
		}
		if (pressed(Controller::MouseButton::MIDDLE)) {
			modelTranslater.y += mouseLocChange.x;
		}
		if (pressed(Controller::MouseButton::RIGHT)) {
			modelTranslater.z += mouseLocChange.x;
		}
	}
	else if ((Controller::Mode) mode == Controller::Mode::VIEWPORT)
	{
		if (pressed(Controller::MouseButton::LEFT)) {
			viewportWidth = glm::clamp(viewportWidth + mouseLocChange.x, -viewportLeftBot.x-1, 1-viewportLeftBot.x);
			viewportHeight = glm::clamp(viewportHeight + mouseLocChange.y, -viewportLeftBot.y-1, 1-viewportLeftBot.y);
		}
	}

	// if (DEBUG_A2) {
	// 	print();
	// }
}

void Controller::reset() {
		modelFrame = mat4(1.0f);
		resetModelAttr();
		resetViewAttr();
		lookFrom = vec3(0.0f, 0.0f, 5.0f);
		lookAt = vec3(0.0f, 0.0f, -1.0f);
		up = vec3(0.0f, 1.0f, 0.0f);
		n = 0.4f;
		f = 20.0f;
		aspectRatio = 1.0f;
		fovy = glm::radians(30.0f);
		viewportLeftBot = vec2(-0.9f, -0.9f);
		viewportWidth = 1.8f;
		viewportHeight = 1.8f;
}

void Controller::resetViewportAttr() {
	viewportLeftBot = lastMouseLoc;
	viewportWidth = 0.0f;
	viewportHeight = 0.0f;
}

void Controller::resetModelAttr() {
	modelScaler = vec3(1.0f, 1.0f, 1.0f);
	modelRotateAngle = vec3(0.0f, 0.0f, 0.0f);
	modelTranslater = vec3(0.0f, 0.0f, 0.0f);
}

void Controller::resetViewAttr() {
	viewRotateAngle = vec3(0.0f, 0.0f, 0.0f);
	viewTranslater = vec3(0.0f, 0.0f, 0.0f);
}

bool Controller::pressed(Controller::MouseButton m) {
	return mouseButtonPressed.count(m) != 0;
}

void Controller::print() {
	cout << "Mode: " << mode << ", modelScaler: " << modelScaler << ", modelRotateAngle: "
	 		 << modelRotateAngle << ", modelTranslater: " << modelTranslater << ", cur mouse: ";
	for (auto mouse : mouseButtonPressed) {
		cout << (int) mouse;
	}
	cout << endl;
}
