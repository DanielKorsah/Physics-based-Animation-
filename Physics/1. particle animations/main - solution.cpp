// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"


// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"


// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void ScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

// Camera
Camera  camera(glm::vec3(0.0f, 5.0f, 20.0f));
double lastX = WIDTH / 2.0;
double lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// view and projection matrices
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

// time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// window
GLFWwindow* window = NULL;

// Moves/alters the camera positions based on user input
void DoMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	double xOffset = xPos - lastX;
	double yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}


void ScrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}


// Renderer initialisation
int initRender() {
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Physics-Based Animation", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	// remove the mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// moder GLEW approach
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 1;
}

// draw mesh
void draw(Mesh mesh)
{
	glm::mat4 model = mesh.getTranslate()* mesh.getRotate() * mesh.getScale();
	Shader shader = mesh.getShader();
	shader.Use();

	// Get the uniform locations
	GLint modelLoc = glGetUniformLocation(shader.Program, "model");
	GLint viewLoc = glGetUniformLocation(shader.Program, "view");
	GLint projLoc = glGetUniformLocation(shader.Program, "projection");

	// Pass the matrices to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(mesh.getVertexArrayObject());
	glDrawArrays(GL_TRIANGLES, 0, mesh.getNumIndices());
	glBindVertexArray(0);
}



// main function
int main()
{
	// init renderer
	initRender();	
			
	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up x5
	plane.scale(glm::vec3(500.0f, 500.0f, 500.0f));

	// create particles
	Mesh particle1 = Mesh::Mesh();
	//scale it down (x.1), translate it up by 2.5 and rotate it by 90 degrees around the x axis
	particle1.translate(glm::vec3(0.0f, 2.5f, 0.0f));
	particle1.scale(glm::vec3(.1f, .1f, .1f));
	particle1.rotate(M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f));
	particle1.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));

	Mesh particle2 = Mesh::Mesh();
	particle2.scale(glm::vec3(.1f, .1f, .1f));
	particle2.rotate(M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f));
	particle2.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_green.frag"));
	glm::vec3 p0 = glm::vec3(0.0f, 5.0f, 0.0f);
	glm::vec3 v0 = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 a = glm::vec3(0.0f, -9.8f, 0.0f);

	// collection of particles
	const int particleNum = 100;
	Mesh particles[particleNum];
	// positions and velocities (acceleration identicle for all particles
	glm::vec3 positions[particleNum];
	glm::vec3 velocities[particleNum];
	// timing for each particle to manage collision with floor
	float firstFrames[particleNum];
	float currentFrames[particleNum];
	srand((unsigned)time(NULL));
	Shader particleShader = Shader("resources/shaders/core.vert", "resources/shaders/core_green.frag");
	for (int i = 0; i < particleNum; i++) {
		particles[i] = Mesh::Mesh();
		particles[i].scale(glm::vec3(.1f, .1f, .1f));
		particles[i].rotate(M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f));
		particles[i].setShader(particleShader);
		positions[i] = glm::vec3(0.0f, 5.0f, 0.0f);
		// generate 3 random numbers to vary velocities
		float angle =  2.0 * M_PI * (float) rand()/RAND_MAX;
		float vx = 1.0f - 2.0f * cos(angle) * (float)rand() / RAND_MAX;
		float vy = 7.0f - 10.0f *(float)rand() / RAND_MAX;
		float vz = 1.0f - 2.0f * sin(angle) * (float)rand() / RAND_MAX;
		velocities[i]= glm::vec3(vx, vy, vz);
		GLfloat ff = glfwGetTime();
		firstFrames[i] = ff;
	}


	GLfloat firstFrame = glfwGetTime();
	
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime() - firstFrame;
		currentFrame *= 1.5f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/*
		**	INTERACTION
		*/

		// Check and call events
		glfwPollEvents();
		DoMovement();

		// view and projection matrices
		projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
		view = camera.GetViewMatrix();

		/*
		**	ANIMATION / SIMULATION
		*/
		
		// 1 - make particle fall at constant speed using translate method
		//particle1.translate(glm::vec3(0.0f, -0.5f * deltaTime, 0.0f));

		// 2 - same as above using setPos method
		//particle2.setPos(glm::vec3(0.0f, .5f - 0.1f * (currentFrame - firstFrame), 0.0f));

		// 3 - make particle oscillate above teh ground plance
		//particle2.setPos(glm::vec3(0.0f, 2.5f + 2.5f * sin(currentFrame - firstFrame), 0.0f));

		// 4 - particle trajectory from initial velocity and acceleration
		//particle2.setPos(p0 + v0 * currentFrame + 0.5f * a * currentFrame * currentFrame);

		// 5 add collision with plane
		/*if (particle2.getTranslate()[3][1] < 0.0f) {
			p0 = particle2.getTranslate()[3];
			p0[1] = 0.0f;
			v0 = v0 + a * currentFrame;
			v0[1] = -.5f * v0[1];
			firstFrame = glfwGetTime();
		}*/

		// 6 collection of particles
		for (int i=0; i< particleNum; i++){
			currentFrames[i] = glfwGetTime() - firstFrames[i];
			currentFrames[i] *= 2.5f;

			particles[i].setPos(positions[i] + velocities[i] * currentFrames[i] + 0.5f * a * currentFrames[i] * currentFrames[i]);
			if (particles[i].getTranslate()[3][1] < 0.0f) {
				positions[i] = particles[i].getTranslate()[3];
				positions[i][1] = 0.0f;
				velocities[i] = velocities[i] + a * currentFrames[i];
				velocities[i][1] = -1.0f * velocities[i][1];
				firstFrames[i] = glfwGetTime();
			}
		}

		/*
		**	RENDER
		*/

		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw objects
		draw(plane);
		//draw(particle2);
		for (int i = 0; i < particleNum; i++) {
			draw(particles[i]);
		}
				

		glBindVertexArray(0);
		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return EXIT_SUCCESS;
}

