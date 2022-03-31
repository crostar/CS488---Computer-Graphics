// Termm-Fall 2020

#include "A5.hpp"
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

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;
const float SCALE_FACTOR = 2.0f;
const float KEY_STEP = 0.01;

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
	glClearColor(0.5, 0.78, 0.79, 1.0);


	createShaderProgram();

	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

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

	m_skybox.init();

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

	initPlanets();

	initParticleGenerator();

	initShadowMapping();
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
	m_shader.attachVertexShader( getAssetFilePath("shadow.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("shadow.fs").c_str() );
	// m_shader.attachFragmentShader( getAssetFilePath("Phong.fs").c_str() );
	// m_shader.attachVertexShader( getAssetFilePath("Phong.vs").c_str() );
	m_shader.link();
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

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
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
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_controller->m_lookFrom = vec3(0.0f, 1.0f, 10.0f);
	m_controller->m_lookAt = vec3(0.0f, 0.0f, 0.0f);
	m_controller->m_view = glm::lookAt(m_controller->m_lookFrom, m_controller->m_lookAt,
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	// m_light.position = vec3(0.0f, 0.0f, 5.0f);
	m_light.position = vec3(0.0f, 0.0f, 0.0f);
	m_light.rgbIntensity = vec3(1.0f); // light
}

//----------------------------------------------------------------------------------------
void A3::buildNodeMaps() {
	SceneNode* root = m_rootNode.get();
	buildNodeMapsRecur(root);
	for (auto it = m_upperJointMap.begin(); it != m_upperJointMap.end(); it++) {
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
	} else if (dynamic_cast<GeometryNode*>(root) == nullptr) {
		isSceneNode = true;
	}

	if (!isSceneNode && m_upperJointMap.count(root->m_nodeId) != 0) {
		for (auto node : root->children) {
			m_upperJointMap[node->m_nodeId] = m_upperJointMap[root->m_nodeId];
		}
	}

	m_nodeMap[root->m_nodeId] = root;
	m_nodeNameMap[root->m_name] = root;

	for (auto node : root->children) {
		buildNodeMapsRecur(node);
	}
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
		m_shader.enable();
		{
			// //-- Set Perpsective matrix uniform for the scene:
			GLint location = m_shader.getUniformLocation("Perspective");
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
			CHECK_GL_ERRORS;

			// // GLint location;
			// //-- Set LightSource uniform for the scene:
			// {
			// 	location = m_shader.getUniformLocation("light.position");
			// 	glUniform3fv(location, 1, value_ptr(m_light.position));
			// 	location = m_shader.getUniformLocation("light.rgbIntensity");
			// 	glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			// 	CHECK_GL_ERRORS;
			// }
			//
			// //-- Set background light ambient intensity
			// {
			// 	location = m_shader.getUniformLocation("ambientIntensity");
			// 	vec3 ambientIntensity(0.25f);
			// 	glUniform3fv(location, 1, value_ptr(ambientIntensity));
			// 	CHECK_GL_ERRORS;
			// }

			//-- Set skybox
			{
				// location = m_shader.getUniformLocation("skybox");
				// glUniform1i(location, 0);
				// CHECK_GL_ERRORS;

				//-- Set texture
				location = m_shader.getUniformLocation("diffuseTexture");
				glUniform1i(location, 0);
				CHECK_GL_ERRORS;
				location = m_shader.getUniformLocation("shadowMap");
				glUniform1i(location, 1);
				CHECK_GL_ERRORS;
			}
		}
		m_shader.disable();
}

//----------------------------------------------------------------------------------------
void A3::initPlanets() {
	Planet* earth = new Planet("earth", dynamic_cast<GeometryNode*>(
		m_nodeNameMap.at("earth")), 0.7f, 4.0f, 1.0f / 365.0f, 1.0f);
	Planet* moon = new Planet("moon", dynamic_cast<GeometryNode*>(
		m_nodeNameMap.at("moon")), 0.3f, 1.5f, 1.0f / 30.0f, 1.0f / 3.0f);
	Planet* sun = new Planet("sun", dynamic_cast<GeometryNode*>(
		m_nodeNameMap.at("sun")), 1.0f, 0.0f, 0.0f, 1.0f / 27.0f);
	moon->m_parent = earth;
	m_planets.emplace("earth", earth);
	m_planets.emplace("moon", moon);
	m_planets.emplace("sun", sun);
	for (auto p : m_planets) {
		p.second->init();
	}

	m_shooting = new ShootingStar("star", dynamic_cast<GeometryNode*>(
		m_nodeNameMap.at("star")), 0.1f, glm::vec3(3.0f, 3.0f, -3.0f), glm::vec3(-1.0f, -1.0f, 1.0f));
	m_shooting->init();
}

//----------------------------------------------------------------------------------------
void A3::initParticleGenerator() {
	m_particle = new ParticleGenerator(500);
}

//----------------------------------------------------------------------------------------
void A3::initShadowMapping() {
	const unsigned int SHADOW_WIDTH = m_windowWidth, SHADOW_HEIGHT = m_windowHeight;
	glGenFramebuffers(1, &m_fbo_depthMap);

	glGenTextures(1, &m_tex_depthMap);
	glBindTexture(GL_TEXTURE_2D, m_tex_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
	             SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT32, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_depthMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_tex_depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create ShaderProgram
	m_depth_shader.generateProgramObject();
	m_depth_shader.attachVertexShader( getAssetFilePath("depth.vs").c_str() );
	m_depth_shader.attachFragmentShader( getAssetFilePath("depth.fs").c_str() );
	m_depth_shader.link();

	// Debug
	m_debugShader.generateProgramObject();
	m_debugShader.attachVertexShader( getAssetFilePath("shadow_debug.vs").c_str() );
	m_debugShader.attachFragmentShader( getAssetFilePath("shadow_debug.fs").c_str() );
	m_debugShader.link();
	m_debugShader.enable();
	GLint location = m_debugShader.getUniformLocation("depthMap");
	glUniform1i(location, 0);
	m_debugShader.disable();
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	m_controller->m_lookFrom += KEY_STEP * m_controller->m_movingDirection;
	m_controller->m_view = glm::lookAt(m_controller->m_lookFrom,
		m_controller->m_lookAt, vec3(0.0f, 1.0f, 0.0f));

	// Update planet location and spins
	for (auto p : m_planets) {
		p.second->update();
	}
	m_shooting->update();

	glm::vec3 offset = - glm::normalize(m_shooting->speed) * m_shooting->radius;
	m_particle->update(0.05, m_shooting, 2, offset);

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
				if (!m_controller->m_operations->redo()) {
					invalidOperation = "redo";
				}
			}
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

	bool showErrorWindow(!invalidOperation.empty());
	if (!invalidOperation.empty()) {
		ImGui::Begin("Error", &showErrorWindow, windowFlags);
		ImGui::Text("Invalid operation, cannot %s", invalidOperation.c_str());
		if( ImGui::Button( "OK" ) ) {
			invalidOperation.clear();
		}
		ImGui::End();
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {
	glEnable( GL_DEPTH_TEST );

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);


	// Set up skybox for objects reflection
	// m_skybox.bindTexture();

	renderSceneGraph(*m_rootNode);

	m_particle->draw(m_controller->m_view, m_perpsective);
	//
	m_skybox.render(m_controller->m_view, m_perpsective);
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(SceneNode & root) {
	const float EPSILON = 1e-3;
	Planet* earth = m_planets[string("earth")];
	glm::vec3 earthLocation(earth->orbitR * glm::cos(earth->rotation),
		0.0f, -earth->orbitR * glm::sin(earth->rotation));
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	// float near_plane = m_planets[string("sun")]->radius + EPSILON;
	// float far_plane =
	// 	m_planets[string("earth")]->orbitR +
	// 	m_planets[string("moon")]->orbitR +
	// 	m_planets[string("moon")]->radius;
	float near_plane = 0.01f;
	float far_plane = 10.0f;

	lightProjection = glm::ortho(-6.0f, 6.0f, -6.0f, 6.0f, near_plane, far_plane);
	// lightProjection = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
	lightView = glm::lookAt(m_light.position, earthLocation, glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;// * m_controller->rootTranslater * m_controller->rootRotater;

	// First pass of shadow mapping: render depth
	renderDepth(root, lightSpaceMatrix);
	// Second pass, do the actual render from view point
	renderFromViewPoint(root, lightSpaceMatrix);
	// debug shadow
	// renderDebug(near_plane, far_plane);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::renderDebug(float near_plane, float far_plane) {
	m_debugShader.enable();
	// GLint location = m_debugShader.getUniformLocation("near_plane");
	// glUniform1f(location, near_plane);
	// CHECK_GL_ERRORS;
	// location = m_debugShader.getUniformLocation("far_plane");
	// glUniform1f(location, far_plane);
	// CHECK_GL_ERRORS;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex_depthMap);
	if (quadVAO == 0)
	{
			float quadVertices[] = {
					// positions        // texture Coords
					-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
					-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
					 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
					 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	m_debugShader.disable();
}

//----------------------------------------------------------------------------------------
void A3::renderFromViewPoint(SceneNode & root, glm::mat4 lightSpaceMatrix) {
	glViewport(0, 0, m_windowWidth, m_windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_tex_depthMap);

	RenderParams params(
		&m_shader, m_controller->rootTranslater * m_controller->rootRotater,
		m_controller->m_view, m_controller->m_lookFrom, m_light.position,
		lightSpaceMatrix, &m_batchInfoMap, false);

	root.render(params);
}

//----------------------------------------------------------------------------------------
void A3::renderDepth(SceneNode & root, glm::mat4 lightSpaceMatrix) {
	// render scene from light's point of view
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_depthMap);
			// glClearDepth(1.0f);
			glClear(GL_DEPTH_BUFFER_BIT);
			// glEnable(GL_POLYGON_OFFSET_FILL);
			// glPolygonOffset(0.2f, 0.4f);

			RenderParams params(
				&m_depth_shader, m_controller->rootTranslater * m_controller->rootRotater,
				m_controller->m_view, m_controller->m_lookFrom, m_light.position,
				lightSpaceMatrix, &m_batchInfoMap, true);

			root.render(params);
			// glDisable(GL_POLYGON_OFFSET_FILL);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, m_windowWidth, m_windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	for (auto p : m_planets) {
		delete p.second;
	}
	delete m_shooting;
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

		if (key == GLFW_KEY_A) {
			m_controller->m_movingDirection.x = -1.0f;
		}
		if (key == GLFW_KEY_D) {
			m_controller->m_movingDirection.x = 1.0f;
		}
		if (key == GLFW_KEY_Q) {
			m_controller->m_movingDirection.y = 1.0f;
		}
		if (key == GLFW_KEY_E) {
			m_controller->m_movingDirection.y = -1.0f;
		}
		if (key == GLFW_KEY_W) {
			m_controller->m_movingDirection.z = -1.0f;
		}
		if (key == GLFW_KEY_S) {
			m_controller->m_movingDirection.z = 1.0f;
		}

		if (key == GLFW_KEY_U) {
			if (!m_controller->m_operations->undo()) {
				invalidOperation = "undo";
			}
		}
		if (key == GLFW_KEY_R) {
			if (!m_controller->m_operations->redo()) {
				invalidOperation = "redo";
			}
		}
		if (key == GLFW_KEY_P) {
			m_controller->mode = (int) Controller::Mode::POSITION;
		}
		if (key == GLFW_KEY_J) {
			m_controller->mode = (int) Controller::Mode::JOINTS;
		}
	}


	if( action == GLFW_RELEASE ) {
		if (key == GLFW_KEY_A) {
			m_controller->m_movingDirection.x = 0.0f;
		}
		if (key == GLFW_KEY_D) {
			m_controller->m_movingDirection.x = 0.0f;
		}
		if (key == GLFW_KEY_Q) {
			m_controller->m_movingDirection.y = 0.0f;
		}
		if (key == GLFW_KEY_E) {
			m_controller->m_movingDirection.y = 0.0f;
		}
		if (key == GLFW_KEY_W) {
			m_controller->m_movingDirection.z = 0.0f;
		}
		if (key == GLFW_KEY_S) {
			m_controller->m_movingDirection.z = 0.0f;
		}
	}

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
// Controller Definitions
Controller::Controller()
	: mode(1),
		lastMouseLoc(0.0f, 0.0f),
		m_trackBall(new TrackBall()),
		m_operations(new OperationStack()),
		m_movingDirection(0.0f)
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
		glm::vec3 axis(0.0f);
		if (pressed(Controller::MouseButton::LEFT)) {
			axis.x = 1.0f;
		}
		if (pressed(Controller::MouseButton::MIDDLE)) {
			axis.y = 1.0f;
		}
		if (pressed(Controller::MouseButton::RIGHT)) {
			axis.z = 1.0f;
		}
		// m_lookAt = vec3(rotate(mat4(1.0f), 0.01f, vec3(0.0f, 1.0f, 0.0f)) * vec4(m_lookAt, 1.0f));
		if (axis != vec3(0.0f)) {
			glm::mat4 rotation = glm::rotate(mat4(1.0f), (float) (2 * PI * mouseLocChange.x), axis);
			glm::vec3 direction = vec3(rotation * vec4((m_lookAt - m_lookFrom), 0.0f));
			m_lookAt = m_lookFrom + direction;
			m_view = glm::lookAt(m_lookFrom, m_lookAt, vec3(0.0f, 1.0f, 0.0f));
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
