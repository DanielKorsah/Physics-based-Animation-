#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"


// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Particle.h"
#include "Body.h"


// time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;



// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 3.0f, 10.0f));

	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up x5
	plane.scale(glm::vec3(5.0f, 5.0f, 5.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));


	// create particle
	std::vector<Particle> particles;
	int particleNum = 300;
	for (int i = 0; i < particleNum; i++)
	{
		Particle p = Particle::Particle();
		particles.push_back(p);
		//scale it down (x.1), translate it up by 2.5 and rotate it by 90 degrees around the x axis
		particles[i].setPos(glm::vec3(0.0f, 4.0f, 0.0f));
		//particles[i].scale(glm::vec3(0.5f, 0.5f, 0.5f));
		//particles[i].rotate((GLfloat) M_PI_2, glm::vec3(0.0f, 1.0f, 0.0f));
		particles[i].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));


		//initial velocty
		particles[i].setVel(glm::vec3(sin(i), 0.0f, cos(i)));
	}

	// time
	GLfloat firstFrame = (GLfloat)glfwGetTime();

	struct Cube
	{
		glm::vec3 origin = glm::vec3(-2.5f, 0.0f, -2.5f);
		glm::vec3 bound = glm::vec3(2.5f, 5.0f, 2.5f);
	};

	Cube cube;
	glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		// Set frame time
		GLfloat currentFrame = (GLfloat)glfwGetTime() - firstFrame;
		// the animation can be sped up or slowed down by multiplying currentFrame by a factor.
		currentFrame *= 1.5f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		for (int i = 0; i < particleNum; i++)
		{

			/*
			**	INTERACTION
			*/
			// Manage interaction
			app.doMovement(deltaTime);


			/*
			**	SIMULATION
			*/
			glm::vec3 Fg;
			float mass = 5;
			Fg = gravity * mass;


			//set acceleration
			particles[i].setAcc(gravity);
			particles[i].setVel(particles[i].getVel() +particles[i].getAcc() * deltaTime);

			//update the particle instance using translate and passing the velocity vector
			particles[i].translate(particles[i].getVel() * deltaTime);


			for (int j = 0; j < 3; j++)
			{
				if (particles[i].getTranslate()[3][j] < cube.origin[j])
				{
					particles[i].setPos(j, cube.origin[j]);
					particles[i].getVel()[j] *= -0.7f;
				}

				if (particles[i].getTranslate()[3][j] > cube.bound[j])
				{
					particles[i].setPos(j, cube.bound[j]);
					particles[i].getVel()[j] *= -0.7;
				}
			}
		}

		/*
		**	RENDER
		*/
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(plane);
		// draw particles
		for (int i = 0; i < particleNum; i++)
		{
			app.draw(particles[i].getMesh());
		}

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

