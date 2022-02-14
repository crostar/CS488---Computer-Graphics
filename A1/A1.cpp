// Termm--Fall 2020

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace glm;
using namespace std;

static const size_t DIM = 16;
static const size_t CUBE_N_VERTS = 3 * 2 * 6;
static const bool DEBUG_A1 = true;

const size_t MAX_CUBE_HEIGHT = 10;
const size_t MIN_CUBE_HEIGHT = 1;
const float MAX_SCALE_FACTOR = 2.0f;
const float MIN_SCALE_FACTOR = 0.5f;
const size_t PERS_THRESH = 2; // Sensitivity of persistence trigger
const float PERS_ROTATION_RATE = 0.01;
const glm::vec2 INVALID_LOC(-1.0f, -1.0f);
const float PI = 3.14159265f;

static bool similar(float a, float b);

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: m_maze(DIM), m_cube_height(MIN_CUBE_HEIGHT),
		m_num_sector(20), m_num_stack(20),
	  m_bg_colour(glm::vec3(0.3f, 0.5f, 0.7f)),
		m_grid_colour(glm::vec3(1.0f, 1.0f, 1.0f)),
		m_cube_colour(glm::vec3(0.0f, 0.0f, 0.0f)),
		m_avatar_colour(glm::vec3(1.0f, 1.0f, 1.0f)),
		m_is_pressing_mouse(false),
		m_persisting(false),
		m_current_mouse_loc(INVALID_LOC),
		m_last_mouse_loc(INVALID_LOC),
		m_h_angle(0.0f),
		m_frames_mouse_hold(PERS_THRESH + 1),
		m_pers_rotation_rate(0.0f),
		m_scale_factor(1.0f),
		current_col( 0 )
{ }

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed=getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;

	// Set the background colour.
	glClearColor( m_bg_colour.r, m_bg_colour.g, m_bg_colour.b, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();
	initCubes();
	initSphere();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt(
		glm::vec3( 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective(
		glm::radians( 30.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A1::initCubes()
{
	float* verts = generateCubeVerts();

	glGenVertexArrays(1, &m_maze_vao);
	glBindVertexArray(m_maze_vao);

	glGenBuffers(1, &m_cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, CUBE_N_VERTS * 3 * sizeof(float),
		verts, GL_STATIC_DRAW);

	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A1::initSphere() {
	float* verts = generateSphereVerts();

	glGenVertexArrays(1, &m_sphere_vao);
	glBindVertexArray(m_sphere_vao);

	glGenBuffers(1, &m_sphere_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * 3 * ((m_num_stack - 1) * m_num_sector * 2) * sizeof(float),
		verts, GL_STATIC_DRAW);

	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}
//----------------------------------------------------------------------------------------
/*
 * Caller is responsible for deallocating the returned result
 */
float* A1::generateSphereVerts() {
	// Generating vertices for a UV-sphere

	float sectorStep = PI / (m_num_sector * 1.0f);
	float stackStep = 2 * PI / (m_num_stack * 1.0f);

	size_t nTriangle = ((m_num_stack - 1) * m_num_sector * 2); // The top/bottom sector contains 1 triangle each piece
	float* verts = new float[3 * 3 * nTriangle];
	size_t ct = 0;

	// Generate top and bottom sector
	for (int j=0; j<m_num_stack; j++) {
		// Top sector
		toCartesian(1, PI / 2, 0.0f, &verts[ct]); ct += 3;
		toCartesian(1, PI / 2 + sectorStep, 0.0f + j * stackStep, &verts[ct]); ct += 3;
		toCartesian(1, PI / 2 + sectorStep, 0.0f + (j - 1) * stackStep, &verts[ct]); ct += 3;
		// Bottom sector
		toCartesian(1, PI / 2 + 2 * PI, 0.0f, &verts[ct]); ct += 3;
		toCartesian(1, PI / 2 + 2 * PI - sectorStep, 0.0f + (j - 1) * stackStep, &verts[ct]); ct += 3;
		toCartesian(1, PI / 2 + 2 * PI - sectorStep, 0.0f + j * stackStep, &verts[ct]); ct += 3;
	}

	// Generate middle sectors
	for (int i=1; i<m_num_sector-1; i++) {
		for (int j=0; j<m_num_stack; j++) {
			float alpha = PI / 2 + i * sectorStep;
			float phi = j * stackStep;
			toCartesian(1, alpha, phi, &verts[ct]); ct += 3;
			toCartesian(1, alpha, phi + stackStep, &verts[ct]); ct += 3;
			toCartesian(1, alpha + sectorStep, phi + stackStep, &verts[ct]); ct += 3;
			toCartesian(1, alpha, phi, &verts[ct]); ct += 3;
			toCartesian(1, alpha + sectorStep, phi + stackStep, &verts[ct]); ct += 3;
			toCartesian(1, alpha + sectorStep, phi, &verts[ct]); ct += 3;
		}
	}

	if (DEBUG_A1) {
		for (int i=0; i<3 * nTriangle; i++) {
			cout << "(" << verts[3*i] << ", " << verts[3*i+1] << ", " << verts[3*i+2] << ")" << endl;
		}
	}

	return verts;
}

//----------------------------------------------------------------------------------------
/*
 * Caller is responsible for deallocating the returned result
 */
float* A1::generateCubeVerts() {
	float* verts = new float[3 * CUBE_N_VERTS];
	size_t ct = 0;
	  // Bottom
		verts[ct] = 0; verts[ct+1] = 0; verts[ct+2] = 0; ct += 3;
		verts[ct] = 1; verts[ct+1] = 0; verts[ct+2] = 0; ct += 3;
		verts[ct] = 1; verts[ct+1] = 0; verts[ct+2] = 1; ct += 3;
		verts[ct] = 0; verts[ct+1] = 0; verts[ct+2] = 0; ct += 3;
		verts[ct] = 1; verts[ct+1] = 0; verts[ct+2] = 1; ct += 3;
		verts[ct] = 0; verts[ct+1] = 0; verts[ct+2] = 1; ct += 3;
		// Top
		verts[ct] = 1; verts[ct+1] = 1; verts[ct+2] = 1; ct += 3;
		verts[ct] = 1; verts[ct+1] = 1; verts[ct+2] = 0; ct += 3;
		verts[ct] = 0; verts[ct+1] = 1; verts[ct+2] = 0; ct += 3;
		verts[ct] = 0; verts[ct+1] = 1; verts[ct+2] = 1; ct += 3;
		verts[ct] = 1; verts[ct+1] = 1; verts[ct+2] = 1; ct += 3;
		verts[ct] = 0; verts[ct+1] = 1; verts[ct+2] = 0; ct += 3;
		// Left
		verts[ct] = 0; verts[ct+1] = 1; verts[ct+2] = 1; ct += 3;
		verts[ct] = 0; verts[ct+1] = 1; verts[ct+2] = 0; ct += 3;
		verts[ct] = 0; verts[ct+1] = 0; verts[ct+2] = 0; ct += 3;
		verts[ct] = 0; verts[ct+1] = 1; verts[ct+2] = 1; ct += 3;
		verts[ct] = 0; verts[ct+1] = 0; verts[ct+2] = 0; ct += 3;
		verts[ct] = 0; verts[ct+1] = 0; verts[ct+2] = 1; ct += 3;
		// Right
		verts[ct] = 1; verts[ct+1] = 0; verts[ct+2] = 0; ct += 3;
		verts[ct] = 1; verts[ct+1] = 1; verts[ct+2] = 0; ct += 3;
		verts[ct] = 1; verts[ct+1] = 1; verts[ct+2] = 1; ct += 3;
		verts[ct] = 1; verts[ct+1] = 0; verts[ct+2] = 1; ct += 3;
		verts[ct] = 1; verts[ct+1] = 0; verts[ct+2] = 0; ct += 3;
		verts[ct] = 1; verts[ct+1] = 1; verts[ct+2] = 1; ct += 3;
		// Back
		verts[ct] = 0; verts[ct+1] = 1; verts[ct+2] = 0; ct += 3;
		verts[ct] = 1; verts[ct+1] = 1; verts[ct+2] = 0; ct += 3;
		verts[ct] = 1; verts[ct+1] = 0; verts[ct+2] = 0; ct += 3;
		verts[ct] = 0; verts[ct+1] = 1; verts[ct+2] = 0; ct += 3;
		verts[ct] = 1; verts[ct+1] = 0; verts[ct+2] = 0; ct += 3;
		verts[ct] = 0; verts[ct+1] = 0; verts[ct+2] = 0; ct += 3;
		// Front
		verts[ct] = 1; verts[ct+1] = 0; verts[ct+2] = 1; ct += 3;
		verts[ct] = 1; verts[ct+1] = 1; verts[ct+2] = 1; ct += 3;
		verts[ct] = 0; verts[ct+1] = 1; verts[ct+2] = 1; ct += 3;
		verts[ct] = 0; verts[ct+1] = 0; verts[ct+2] = 1; ct += 3;
		verts[ct] = 1; verts[ct+1] = 0; verts[ct+2] = 1; ct += 3;
		verts[ct] = 0; verts[ct+1] = 1; verts[ct+2] = 1; ct += 3;

	if (DEBUG_A1) {
		for (int i=0; i<CUBE_N_VERTS; i++) {
			cout << "(" << verts[3*i] << ", " << verts[3*i+1] << ", " << verts[3*i+2] << ")" << endl;
		}
	}
	return verts;
}
//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
	updatePersistenceLogic();
}

//----------------------------------------------------------------------------------------
/*
 * Update persistence logic: we record the latest duration of pressing mouse while
 * not moving it. The persistence is triggered when this duration is short enough, i.e.
 * the mouse is still "moving" upon release. We also record the last non-zero movement of mouse
 * between two frames. The persistence rotation rate is determined by this movement, i.e.
 # the speed of mouse upon release.
 */
void A1::updatePersistenceLogic() {
	if (DEBUG_A1) {
		cout << "Clicking: " << m_is_pressing_mouse << ", persisting: " << m_persisting
			<< ", m_frames_mouse_hold: " << m_frames_mouse_hold << ", m_current_mouse_loc: "
			<< m_current_mouse_loc.x << ", m_last_mouse_loc: " << m_last_mouse_loc.x << endl;
	}

	if (!m_is_pressing_mouse) {
		if (m_frames_mouse_hold < PERS_THRESH) {
			m_persisting = true;
			m_frames_mouse_hold = PERS_THRESH + 1;
		}
	} else {
		m_persisting = false;
		if (similar(m_current_mouse_loc.x, m_last_mouse_loc.x)) {
			m_frames_mouse_hold += 1;
		} else {
			m_frames_mouse_hold = 0;
			// Update mouse release speed and calculate corresponding rotation rate
			m_pers_rotation_rate = distanceToAngle(m_current_mouse_loc.x - m_last_mouse_loc.x);
			m_last_mouse_loc = m_current_mouse_loc;
		}
	}

	if (m_persisting) {
		m_h_angle += m_pers_rotation_rate;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		if( ImGui::Button( "Reset" ) ) {
			reset();
		}
		ImGui::SameLine();
		if (ImGui::Button( "Dig" )) {
			m_maze.digMaze();
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		ImGui::PushID( 0 );
		if (current_col == 0) {
			ImGui::ColorEdit3( "Colour", glm::value_ptr(m_bg_colour) );
		} else if (current_col == 1) {
			ImGui::ColorEdit3( "Colour", glm::value_ptr(m_cube_colour) );
		} else if (current_col == 2) {
			ImGui::ColorEdit3( "Colour", glm::value_ptr(m_avatar_colour) );
		}
		ImGui::SameLine();
		ImGui::RadioButton( "background", &current_col, 0 );
		ImGui::SameLine();
		ImGui::RadioButton( "cubes", &current_col, 1 );
		ImGui::SameLine();
		ImGui::RadioButton( "avatar", &current_col, 2 );
		// Update Color information
		ImGui::PopID();


		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}


		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	const glm::vec3 y_axis(0.0f, 1.0f, 0.0f);
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
	// Handle the rotation by adjusting Model matrix
	W = glm::rotate(mat4(), m_h_angle, y_axis) * W;
	// W = glm::rotate(mat4(), m_h_angle + m_h_angle_acc, y_axis) * W;
	W = glm::scale(mat4(), vec3(m_scale_factor)) * W;

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Draw the background
		glClearColor( m_bg_colour.r, m_bg_colour.g, m_bg_colour.b, 1.0 );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, m_grid_colour.r, m_grid_colour.g, m_grid_colour.b );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		glBindVertexArray(m_maze_vao);
		glUniform3f(col_uni, m_cube_colour.r, m_cube_colour.g, m_cube_colour.b );
		for (size_t i=0; i<m_maze.getDim(); i++) {
			for (size_t j=0; j<m_maze.getDim(); j++) {
				if (m_maze.getValue(i, j) == 1) {
					for (size_t k=0; k<m_cube_height; k++) {
						mat4 Wi = glm::translate( W, vec3( j * 1.0f, k * 1.0f, i * 1.0f ) );
						glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( Wi ) );
						glDrawArrays(GL_TRIANGLES, 0, CUBE_N_VERTS);
					}
				}
			}
		}

		// Draw the Avatar
		glBindVertexArray( m_sphere_vao );
		mat4 transform = glm::translate(W, vec3(0.5f, 0.5f, 0.5f));
		transform *= glm::translate(mat4(), vec3(m_maze.getAvatarCol() * 1.0f,
																						 0.0f,
																		  		 	 m_maze.getAvatarRow() * 1.0f));
		transform *= glm::scale(mat4(), vec3(0.3f));
		glUniform3f(col_uni, m_avatar_colour.r, m_avatar_colour.g, m_avatar_colour.b);
		glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, 0, ((m_num_stack - 1) * m_num_sector * 2) * 3);

		// Highlight the active square.
	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos)
{
	bool eventHandled(false);

	// Translate current mouse position to GL coordinates:
	m_last_mouse_loc = m_current_mouse_loc;
	m_current_mouse_loc = vec2 (
			(2.0f * xPos) / m_windowWidth - 1.0f,
			1.0f - ( (2.0f * yPos) / m_windowHeight)
	);

	if (m_is_pressing_mouse) {
		m_h_angle += distanceToAngle(m_current_mouse_loc.x - m_last_mouse_loc.x);
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (actions == GLFW_PRESS) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
			m_is_pressing_mouse = true;
		}
	}

	if (actions == GLFW_RELEASE) {
		m_is_pressing_mouse = false;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	if (DEBUG_A1) {
		cout << "xOffset: " << xOffSet << ", yOffset: " << yOffSet
		 << "m_scale_factor before update: " << m_scale_factor << endl;
	}

	float newS = m_scale_factor + yOffSet / 10.0f;
	if (newS <= MAX_SCALE_FACTOR && newS >= MIN_SCALE_FACTOR) {
		m_scale_factor = newS;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			m_maze.setMoveState(Maze::MoveState::Normal);
			eventHandled = true;
		}
	}

	if( action == GLFW_PRESS ) {
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}

		if (key == GLFW_KEY_R) {
			reset();
			eventHandled = true;
		}

		if (key == GLFW_KEY_D) {
			m_maze.digMaze();
			eventHandled = true;
		}

		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			m_maze.setMoveState(Maze::MoveState::Dashing);
			eventHandled = true;
		}

		if (key == GLFW_KEY_SPACE) {
			// Increase cube height by one unit
			m_cube_height = std::min(MAX_CUBE_HEIGHT, m_cube_height+1);
			eventHandled = true;
		}

		if (key == GLFW_KEY_BACKSPACE) {
			// Decrease cube height by one unit
			m_cube_height = std::max(MIN_CUBE_HEIGHT, m_cube_height-1);
			eventHandled = true;
		}

		if (key == GLFW_KEY_LEFT) {
			// Move Avatar to the left by 1 unit
			m_maze.move(Maze::Direction::Left);
			eventHandled = true;
		}

		if (key == GLFW_KEY_RIGHT) {
			// Move Avatar to the right by 1 unit
			m_maze.move(Maze::Direction::Right);
			eventHandled = true;
		}

		if (key == GLFW_KEY_UP) {
			// Move Avatar up by 1 unit
			m_maze.move(Maze::Direction::Up);
			eventHandled = true;
		}

		if (key == GLFW_KEY_DOWN) {
			// Move Avatar down by 1 unit
			m_maze.move(Maze::Direction::Down);
			eventHandled = true;
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Restrose the grid to its initial, empty state
 * resets the view to the default
 * resets the colour to the initial colours
 * moves the avatar back to the cell (0, 0)
 */
 void A1::reset() {
	 m_maze.reset();
	 m_cube_height = 1;
	 m_bg_colour = vec3(0.3f, 0.5f, 0.7f);
	 m_grid_colour = vec3(1.0f, 1.0f, 1.0f);
	 m_cube_colour = vec3(0.0f, 0.0f, 0.0f);
	 m_avatar_colour = vec3(1.0f, 1.0f, 1.0f);
	 current_col = 0;
	 m_persisting = false;
	 m_h_angle = 0.0f;
	 m_scale_factor = 1.0f;
 }

//----------------------------------------------------------------------------------------
/*
 * Translate the horizontal distance into the rotation angle, i.e. mapping the
 * screen width to (-2PI, 2PI)
 */
 float A1::distanceToAngle(float dist) {
	 // if (DEBUG_A1) {
		//  cout << "Distance to Angle, dist: " << dist << ", m_windowWidth: " << m_windowWidth
		//  	<< ", result: " << dist * (8 * PI) << endl;
	 // }
	 return dist * (2 * PI);
 }

 //----------------------------------------------------------------------------------------
 /*
  * Test if two floats are similar enough to be considered as equal
  */
	bool similar(float a, float b) {
		float epsilon = 1e-8;
		return abs(a - b) < epsilon;
	}


	//----------------------------------------------------------------------------------------
	/*
	 * Translate spherical coordinates to Cartesian coordinates
	 * And fill the result into the destination
	 */
	 void A1::toCartesian(float r, float alpha, float phi, float* dest) {
		 dest[0] = r * glm::cos(alpha) * glm::cos(phi);
	 	 dest[1] = r * glm::sin(alpha);
		 dest[2] = r * glm::cos(alpha) * glm::sin(phi);
	 }
