// Termm-Fall 2020

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;
const float SCALE_FACTOR = 2.0f;

const size_t DIM = 16;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
		m_controller(new Controller())
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.8, 0.9, 1.0, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	initFbo();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	buildNodeMaps();
	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	// m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	// m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.attachVertexShader( getAssetFilePath("Phong.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("Phong.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();

	m_shader_picking.generateProgramObject();
	m_shader_picking.attachVertexShader( getAssetFilePath("pick_VertexShader.vs").c_str() );
	m_shader_picking.attachFragmentShader( getAssetFilePath("pick_FragmentShader.fs").c_str() );
	m_shader_picking.link();
}

//----------------------------------------------------------------------------------------
void A3::initFbo()
{
	glGenFramebuffers(1, &m_picking_fbo);
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 1.0f, 6.0f), vec3(0.0f, 1.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	// m_light.position = vec3(0.0f, 0.0f, 5.0f);
	m_light.position = vec3(-5.0f, 0.5f, 5.0f);
	m_light.rgbIntensity = vec3(1.0f); // light
}

//----------------------------------------------------------------------------------------
void A3::buildNodeMaps() {
	SceneNode* root = m_rootNode.get();
	buildNodeMapsRecur(root);
	for (auto it = m_upperJointMap.begin(); it != m_upperJointMap.end(); it++) {
		cout << "it " << it->first << ", " << it->second << endl;
		if (it->second == nullptr) {
			m_upperJointMap.erase(it);
		}
	}

	if (DEBUG_A3) {
		cout << "Joint map built: " << m_upperJointMap.size() << endl;
		for (auto kv : m_upperJointMap) {
			std::cout << "( " << kv.first << "," << kv.second << ")" << endl;
		}
	}

	for (auto it = m_upperJointMap.begin(); it != m_upperJointMap.end(); it++) {
		if (m_jointGroupMap.count(it->second->m_nodeId) != 0) {
			m_jointGroupMap[it->second->m_nodeId].push_back(m_nodeMap[it->first]);
		} else {
			m_jointGroupMap[it->second->m_nodeId] = std::list<SceneNode*>({m_nodeMap[it->first]});
		}
	}

	if (DEBUG_A3) {
		cout << "Joint group map built: " << m_jointGroupMap.size() << endl;
		for (auto kv : m_jointGroupMap) {
			std::cout << "Joint " << m_nodeMap[kv.first]->m_name << "(" << kv.first << "):" << endl;
			for (auto node : kv.second) {
				std::cout << "  " << node->m_name << "(" << node << ")" << endl;
			}
		}
	}
}

void A3::buildNodeMapsRecur(SceneNode* root) {
	bool isSceneNode = false;
	if (dynamic_cast<JointNode*>(root) != nullptr) {
		m_upperJointMap[root->m_nodeId] = root;
		cout << "Upper joint of " << root->m_name << "(" << root->m_nodeId << ")" << ": " << root->m_name << endl;
	} else if (dynamic_cast<GeometryNode*>(root) == nullptr) {
		isSceneNode = true;
	}

	if (!isSceneNode && m_upperJointMap.count(root->m_nodeId) != 0) {
		for (auto node : root->children) {
			m_upperJointMap[node->m_nodeId] = m_upperJointMap[root->m_nodeId];
			cout << "Upper joint of " << node->m_name << "(" << node->m_nodeId << ")"
				<< ": " << m_upperJointMap[root->m_nodeId]->m_name << endl;
		}
	}

	m_nodeMap[root->m_nodeId] = root;

	for (auto node : root->children) {
		buildNodeMapsRecur(node);
	}
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	// if (m_controller->picking) {
		m_shader_picking.enable();
		{
			//-- Set Perpsective matrix uniform for the scene:
			GLint location = m_shader_picking.getUniformLocation("Perspective");
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
			CHECK_GL_ERRORS;
		}
		m_shader_picking.disable();
	// } else {
		m_shader.enable();
		{
			//-- Set Perpsective matrix uniform for the scene:
			GLint location = m_shader.getUniformLocation("Perspective");
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
			CHECK_GL_ERRORS;


			//-- Set LightSource uniform for the scene:
			{
				location = m_shader.getUniformLocation("light.position");
				glUniform3fv(location, 1, value_ptr(m_light.position));
				location = m_shader.getUniformLocation("light.rgbIntensity");
				glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
				CHECK_GL_ERRORS;
			}

			//-- Set background light ambient intensity
			{
				location = m_shader.getUniformLocation("ambientIntensity");
				vec3 ambientIntensity(0.25f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}
		m_shader.disable();
	// }
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Application")) {
			if (ImGui::MenuItem("Reset Position (I)")) {
				m_controller->resetPosition();
			}
			if (ImGui::MenuItem("Reset Orientation (O)")) {
				m_controller->resetOrientation();
			}
			if (ImGui::MenuItem("Reset Joints (S)")) {
				m_controller->resetJoints();
			}
			if (ImGui::MenuItem("Reset All (A)")) {
				m_controller->resetAll();
			}
			if( ImGui::MenuItem( "Quit Application" ) ) {
				glfwSetWindowShouldClose(m_window, GL_TRUE);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo (U)")) {
				if (!m_controller->m_operations->undo()) {
					invalidOperation = "undo";
				}
			}
			if (ImGui::MenuItem("Redo (R)")) {
				if (m_controller->m_operations->redo()) {
					invalidOperation = "redo";
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options")) {
			if (ImGui::MenuItem("Circle (C)")) {}
			if (ImGui::MenuItem("Z-buffer (Z)")) {}
			if (ImGui::MenuItem("Backface culling (B)")) {}
			if (ImGui::MenuItem("Frontface culling (F)")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);
	ImGui::RadioButton( "Position/Orientation (P)", &(m_controller->mode), 1 );
	ImGui::RadioButton( "Joints (J)", &(m_controller->mode), 2 );
	ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
	ImGui::End();

	if (!invalidOperation.empty()) {
		ImGui::Begin("Error");
		ImGui::Text("Invalid operation, cannot %s", invalidOperation.c_str());
		ImGui::End();
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	glDisable( GL_DEPTH_TEST );
	renderArcCircle();

	glEnable( GL_DEPTH_TEST );
	renderSceneGraph(*m_rootNode);

}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// cout << "Rendering Scene Graph, picking: " << m_controller->picking << endl;
	RenderParams params(
		m_controller->picking ? &m_shader_picking : &m_shader,
		m_controller->rootTranslater * m_controller->rootRotater,
		m_view, &m_batchInfoMap, m_controller->picking);

	// if (m_controller->picking) {
	// 	glBindFramebuffer(GL_FRAMEBUFFER, m_picking_fbo);
	// } else {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// }
	root.render(params);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Update model, called after mouse events
 */
void A3::updateModel() {
	m_controller->rootTranslater =
		glm::translate(glm::mat4(), SCALE_FACTOR * m_controller->modelTranslater) *
		m_controller->rootTranslater;
	m_controller->rootRotater = m_controller->m_trackBall->m_rotMat * m_controller->rootRotater;
	// m_rootNode->set_transform(m_rootNode->get_transform() * m_controller->m_trackBall->m_rotMat);
	// m_rootNode->translate(SCALE_FACTOR * m_controller->modelTranslater);
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{
	glDeleteFramebuffers(1, &m_picking_fbo);
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// if (DEBUG_A3) {
	// 	cout << "Prev: " << m_controller->lastMouseLoc << " Cur: " << vec2(xPos, yPos) << endl;
	// }

	float fDiameter = (m_windowWidth < m_windowHeight) ? m_windowWidth * 0.5 : m_windowHeight * 0.5;
	vec2 center(m_windowWidth / 2.0f, m_windowHeight / 2.0f);

	// Update controller
	m_controller->updateUponMouseMoveEvent(vec2(xPos, yPos), fDiameter, center, m_nodeMap);
	m_controller->lastMouseLoc = vec2(xPos, yPos);

	// Update model in terms of controller
	updateModel();

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
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
			} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				m_controller->mouseButtonPressed.insert(Controller::MouseButton::RIGHT);
			} else if (button = GLFW_MOUSE_BUTTON_MIDDLE) {
				m_controller->mouseButtonPressed.insert(Controller::MouseButton::MIDDLE);
			}
		}
		if (actions == GLFW_RELEASE) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				m_controller->mouseButtonPressed.erase(Controller::MouseButton::LEFT);
			} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				m_controller->mouseButtonPressed.erase(Controller::MouseButton::RIGHT);
			} else if (button = GLFW_MOUSE_BUTTON_MIDDLE) {
				m_controller->mouseButtonPressed.erase(Controller::MouseButton::MIDDLE);
			}
		}
	}

	// Handle picking
	double xpos = 0.0f, ypos = 0.0f;
	glfwGetCursorPos( m_window, &xpos, &ypos );
	if (
		(Controller::Mode) m_controller->mode == Controller::Mode::JOINTS &&
		button == GLFW_MOUSE_BUTTON_LEFT &&
		actions == GLFW_PRESS)
	{
		m_controller->picking = true;
		uploadCommonSceneUniforms();
		glClearColor(1.0, 1.0, 1.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glClearColor(0.8, 0.9, 1.0, 1.0);

		draw();

		// Ugly -- FB coordinates might be different than Window coordinates
		// (e.g., on a retina display).  Must compensate.
		xpos *= double(m_framebufferWidth) / double(m_windowWidth);
		// WTF, don't know why I have to measure y relative to the bottom of
		// the window in this case.
		ypos = m_windowHeight - ypos;
		ypos *= double(m_framebufferHeight) / double(m_windowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
		// A bit ugly -- don't want to swap the just-drawn false colours
		// to the screen, so read from the back buffer.
		glReadBuffer( GL_BACK );
		// Actually read the pixel at the mouse location.
		glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
		CHECK_GL_ERRORS;
		CHECK_FRAMEBUFFER_COMPLETENESS;

		// Reassemble the object ID.
		unsigned int pickedId = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);
		if (DEBUG_A3) {
			cout << "Picked RGB (" << (float) buffer[0] << ", " << (float) buffer[1]
			 << ", " << (float) buffer[2] << ")" << " ID " << pickedId << endl;
		}

		if (m_upperJointMap.count(pickedId) != 0) {
			SceneNode* jointSelected = m_upperJointMap[pickedId];
			for (auto node : m_jointGroupMap[jointSelected->m_nodeId]) {
				node->isSelected = !(node->isSelected);
			}
			if (DEBUG_A3) {
				cout << "Picking node: " << pickedId << ", joint " << m_upperJointMap[pickedId]->m_name
				  	 << "selected status: " << m_upperJointMap[pickedId]->isSelected << endl;
			}
		}

		m_controller->picking = false;

		CHECK_GL_ERRORS;
	}

	if (
		(Controller::Mode) m_controller->mode == Controller::Mode::JOINTS &&
		actions == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && !m_controller->middleMouseOperations.empty()) {
			m_controller->m_operations->addOperations(m_controller->middleMouseOperations);
			m_controller->middleMouseOperations.clear();
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && !m_controller->middleMouseOperations.empty()) {
			m_controller->m_operations->addOperations(m_controller->rightMouseOperations);
			m_controller->rightMouseOperations.clear();
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		if (key == GLFW_KEY_I) {
			m_controller->resetPosition();
		}
		if (key == GLFW_KEY_O) {
			m_controller->resetOrientation();
		}
		if (key == GLFW_KEY_S) {
			m_controller->resetJoints();
		}
		if (key == GLFW_KEY_A) {
			m_controller->resetAll();
		}
		if (key == GLFW_KEY_U) {
			if (m_controller->m_operations->undo()) {
				invalidOperation = "undo";
			}
		}
		if (key == GLFW_KEY_R) {
			if (m_controller->m_operations->redo()) {
				invalidOperation = "redo";
			}
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
// Controller Definitions
Controller::Controller()
	: mode(1),
		picking(false),
		lastMouseLoc(0.0f, 0.0f),
		m_trackBall(new TrackBall()),
		m_operations(new OperationStack())
	{
		reset();
	}

// Parse the mouse input into the controller
void Controller::updateUponMouseMoveEvent(vec2 mousePos, float fDiameter, vec2 center,
	 	std::unordered_map<unsigned int, SceneNode*> & nodeMap) {
	if (!pressed(Controller::MouseButton::LEFT)) {
		modelTranslater = vec3(0.0f);
	}
	if (!pressed(Controller::MouseButton::RIGHT)) {
		m_trackBall->reset();
	}

	vec2 mouseLocChange = pixelToFC(mousePos, center) - pixelToFC(lastMouseLoc, center);
	if ((Controller::Mode) mode == Controller::Mode::POSITION)
	{
		// Fill in with event handling code...
		if (pressed(Controller::MouseButton::LEFT)) {
			modelTranslater.x = mouseLocChange.x;
			modelTranslater.y = mouseLocChange.y;
		}
		if (pressed(Controller::MouseButton::MIDDLE)) {
			modelTranslater.z = mouseLocChange.y;
		}
		if (pressed(Controller::MouseButton::RIGHT)) {
			m_trackBall->update(mousePos - center, lastMouseLoc - center, fDiameter);
		}
	}
	else if ((Controller::Mode) mode == Controller::Mode::JOINTS)
	{
		std::vector<SceneNode*> selected;
		if (pressed(Controller::MouseButton::MIDDLE)) {
			vec2 rotateAngle = vec2(mouseLocChange.x * 100.0f, 0.0f);
			for (auto kv : nodeMap) {
				cout << "Processing kv " << kv.first << ", " << kv.second << ", " << kv.second->m_name << endl;
				JointNode* jointNode = dynamic_cast<JointNode*>(kv.second);
				if (jointNode != nullptr && jointNode->isSelected) {
					selected.push_back(kv.second);
				}
			}
			middleMouseOperations.emplace_back(selected, rotateAngle);
			middleMouseOperations.back().execute();
		}
		if (pressed(Controller::MouseButton::RIGHT)) {
			vec2 rotateAngle = vec2(0.0f, mouseLocChange.y * 100.0f);
			for (auto kv : nodeMap) {
				JointNode* jointNode = dynamic_cast<JointNode*>(kv.second);
				if (jointNode != nullptr && jointNode->isSelected && jointNode->isHeadJoint()) {
					selected.push_back(kv.second);
				}
			}
			rightMouseOperations.emplace_back(selected, rotateAngle);
			rightMouseOperations.back().execute();
		}
	}

	// if (DEBUG_A3) {
	// 	print();
	// }
}

bool Controller::pressed(Controller::MouseButton m) {
	return mouseButtonPressed.count(m) != 0;
}

void Controller::reset() {
	modelTranslater = vec3(0.0f);
}

void Controller::print() {
	cout << "modelTranslater: " << modelTranslater << endl;
}


void Controller::resetPosition() {
	rootTranslater = glm::mat4();
}

void Controller::resetOrientation() {
	rootRotater = glm::mat4();
}

void Controller::resetJoints() {
	m_operations->clear();
}

void Controller::resetAll() {
	resetPosition();
	resetOrientation();
	resetJoints();
}


// Helpers
vec2 Controller::pixelToFC(vec2 loc, vec2 center) {
	vec2 res (
			loc.x / center.x - 1.0f,
			1.0f - ( loc.y / center.y )
	);
	// if (DEBUG_A3) {
	// 	cout << "Frame coordinate: " << res << endl;
	// }
	return res;
}
