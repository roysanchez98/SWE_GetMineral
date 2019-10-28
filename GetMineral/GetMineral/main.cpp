#include <iostream>
#include <sstream> //for fps counter

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"

#include "src/shaderProgram.h"
#include "src/texture2D.h"
#include "src/camera.h"

#include "constants.h"

//class variables
GLFWwindow* pWindow = NULL; //pointer to window
FPSCamera fpsCam(glm::vec3(0.0f, 0.0f, 5.0f));
const string grass_side = "res/textures/grass_side.png";
const string floor_image = "res/textures/grid.jpg";


//func declarations
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFrameBufferSiz(GLFWwindow *window, int width, int height);
void glfw_onMouseScroll(GLFWwindow *window, double deltaX, double deltaY);
void update(double elapsedTime);
void showFPS(GLFWwindow* window);
bool initOpenGL();

using namespace std;

int main()
{

	/********* INIT *********/
	if (!initOpenGL())
	{
		cerr << "GLFW initialization failed." << endl;
		return -1;
	}

	GLfloat vertices[] = {
		//position X, Y, Z     //texture coords

		// front face
		-1.0f,  1.0f,  1.0f,	0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,	1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,	1.0f, 0.0f,

		 // back face
		 -1.0f,  1.0f, -1.0f,	0.0f, 1.0f,
		  1.0f, -1.0f, -1.0f,	1.0f, 0.0f,
		  1.0f,  1.0f, -1.0f,	1.0f, 1.0f,
		 -1.0f,  1.0f, -1.0f,	0.0f, 1.0f,
		 -1.0f, -1.0f, -1.0f,	0.0f, 0.0f,
		  1.0f, -1.0f, -1.0f,	1.0f, 0.0f,

		  // left face
		  -1.0f,  1.0f, -1.0f,	0.0f, 1.0f,
		  -1.0f, -1.0f,  1.0f,	1.0f, 0.0f,
		  -1.0f,  1.0f,  1.0f,	1.0f, 1.0f,
		  -1.0f,  1.0f, -1.0f,	0.0f, 1.0f,
		  -1.0f, -1.0f, -1.0f,	0.0f, 0.0f,
		  -1.0f, -1.0f,  1.0f,	1.0f, 0.0f,

		  // right face
		   1.0f,  1.0f,  1.0f,	0.0f, 1.0f,
		   1.0f, -1.0f, -1.0f,	1.0f, 0.0f,
		   1.0f,  1.0f, -1.0f,	1.0f, 1.0f,
		   1.0f,  1.0f,  1.0f,	0.0f, 1.0f,
		   1.0f, -1.0f,  1.0f,	0.0f, 0.0f,
		   1.0f, -1.0f, -1.0f,	1.0f, 0.0f,

		   // top face
		   -1.0f,  1.0f, -1.0f,	0.0f, 1.0f,
			1.0f,  1.0f,  1.0f,	1.0f, 0.0f,
			1.0f,  1.0f, -1.0f,	1.0f, 1.0f,
		   -1.0f,  1.0f, -1.0f,	0.0f, 1.0f,
		   -1.0f,  1.0f,  1.0f,	0.0f, 0.0f,
			1.0f,  1.0f,  1.0f,	1.0f, 0.0f,

			// bottom face
			-1.0f, -1.0f,  1.0f,	0.0f, 1.0f,
			 1.0f, -1.0f, -1.0f,	1.0f, 0.0f,
			 1.0f, -1.0f,  1.0f,	1.0f, 1.0f,
			-1.0f, -1.0f,  1.0f,	0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,
			 1.0f, -1.0f, -1.0f,	1.0f, 0.0f,
	};


	glm::vec3 cubePos = glm::vec3(0.0f, 0.0f, -5.0f);
	glm::vec3 floorPos = glm::vec3(0.0f, -1.0f, 0.0f);

	//create vertex buffer object (VBO)
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//create vertex array object (VAO)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//define vertex format for the mesh vertices
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);

	//tex coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	//create Shaders
	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("basic.vert", "basic.frag");

	Texture2D texture;
	texture.loadTexture(grass_side, true);

	Texture2D floorTexture;
	floorTexture.loadTexture(floor_image, true);

	double lastTime = glfwGetTime();


	/********* Main Game Loop *********/
	while (!glfwWindowShouldClose(pWindow))
	{
		showFPS(pWindow);
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		/********* Handle Inputs *********/
		glfwPollEvents();
		update(deltaTime);

		/********* Update/Draw to Window *********/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		texture.bind(0);

		glm::mat4 model, view, projection;

		model = glm::translate(model, cubePos);

		view = fpsCam.getViewMatrix();


		projection = glm::perspective(glm::radians(fpsCam.getFOV()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		shaderProgram.use();

		// Pass the matrices to the shader
		shaderProgram.setUniform("model", model);
		shaderProgram.setUniform("view", view);
		shaderProgram.setUniform("projection", projection);


		glBindVertexArray(vao); //bind vao
			// Draw cube
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Position and render the floor (a squashed and scaled cube!)
		// Make the floor texture "active" in the shaders
		floorTexture.bind(0);
		model = glm::translate(model, floorPos) * glm::scale(model, glm::vec3(10.0f, 0.01f, 10.0f));

		// Send the model matrix for the floor to the vertex shader
		shaderProgram.setUniform("model", model);

		// Draw floor
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0); //unbind
		glfwSwapBuffers(pWindow); //double buffer -> eliminates screen flicker

		lastTime = currentTime;
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glfwTerminate();
	return 0;
}

//func implementations

bool initOpenGL() {

	/********* INIT *********/

	//initialize GLFW
	if (!glfwInit())
	{
		cerr << "GLFW initialization failed" << endl;
		return false;
	}

	/********* Settings *********/
	//set version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//set modern opengl
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//set compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/********* Create Window *********/
	//Fullscreen
	if (G_FULLSCREEN)
	{
		GLFWmonitor* pMonitor = glfwGetPrimaryMonitor(); //get pointer to primary monitor
		const GLFWvidmode* pVmode = glfwGetVideoMode(pMonitor);
		if (pVmode != NULL)
		{
			pWindow = glfwCreateWindow(pVmode->width, pVmode->height, APP_TITLE, pMonitor, NULL);
		}
	}

	//Default
	else {
		pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_TITLE, NULL, NULL);
	}

	if (pWindow == NULL)
	{
		cerr << "Failed to create GLFW Window" << endl;
		glfwTerminate();
		return false;
	}

	//if window creation successful, make current window
	glfwMakeContextCurrent(pWindow);
	glfwSetKeyCallback(pWindow, glfw_onKey);
	glfwSetScrollCallback(pWindow, glfw_onMouseScroll);

	//Hides and grabs cursor, unlimited movement
	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(pWindow, WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0);

	//initialize GLEW on window creation
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW initialization failed." << endl;
		return false;
	}

	//set screen color
	glClearColor(0.23f, 0.38f, 0.47f, 1.0f); //RGBAlpha color channels, from 0 to 1
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//exit on esc
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		G_WIREFRAME = !G_WIREFRAME;
		if (G_WIREFRAME)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

}

void glfw_onMouseScroll(GLFWwindow *window, double deltaX, double deltaY)
{
	double fov = fpsCam.getFOV() + deltaY * ZOOM_SENSITIVITY;

	fov = glm::clamp(fov, 1.0, 120.0);

	fpsCam.setFOV((float)fov);
}

void update(double elapsedTime)
{
	// Camera orientation
	double mouseX, mouseY;

	// Get the current mouse cursor position delta
	glfwGetCursorPos(pWindow, &mouseX, &mouseY);

	// Rotate the camera the difference in mouse distance from the center screen.  Multiply this delta by a speed scaler
	fpsCam.rotate((float)(WINDOW_WIDTH / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(WINDOW_HEIGHT / 2.0 - mouseY) * MOUSE_SENSITIVITY);

	// Clamp mouse cursor to center of screen
	glfwSetCursorPos(pWindow, WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0);

	// Camera FPS movement

	// Forward/backward
	if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
		fpsCam.move(MOVE_SPEED * (float)elapsedTime * fpsCam.getLook());
	else if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)
		fpsCam.move(MOVE_SPEED * (float)elapsedTime * -fpsCam.getLook());

	// Strafe left/right
	if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS)
		fpsCam.move(MOVE_SPEED * (float)elapsedTime * -fpsCam.getRight());
	else if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS)
		fpsCam.move(MOVE_SPEED * (float)elapsedTime * fpsCam.getRight());

	// Up/down
	if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS)
		fpsCam.move(MOVE_SPEED * (float)elapsedTime * fpsCam.getUp());
	else if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
		fpsCam.move(MOVE_SPEED * (float)elapsedTime * -fpsCam.getUp());
}

void glfw_onFrameBufferSiz(GLFWwindow *window, int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

}


void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); //returns number of seconds since GLFW started as double

	elapsedSeconds = currentSeconds - previousSeconds;

	//limit text update 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		ostringstream outs;
		outs.precision(3);
		outs << fixed
			<< APP_TITLE << "  "
			<< "FPS: " << fps << "  "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		frameCount = 0; //reset
	}
	frameCount++; //tick frames
}