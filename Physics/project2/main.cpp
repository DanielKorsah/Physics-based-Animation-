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
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));
			
	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up x5
	plane.scale(glm::vec3(5.0f, 5.0f, 5.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));


	// create particle
	std::vector<Particle> particles;
	int particleNum = 10;
	for (int i = 0; i < particleNum; i++) 
	{
		Particle p = Particle::Particle();
		particles.push_back(p);
		//scale it down (x.1), translate it up by 2.5 and rotate it by 90 degrees around the x axis
		particles[i].translate(glm::vec3(0.0f, 5.0f, 0.0f));
		particles[i].scale(glm::vec3(0.5f, 0.5f, 0.5f));
		particles[i].rotate((GLfloat) M_PI_2, glm::vec3(0.0f, 1.0f, 0.0f));
		particles[i].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));
		
	}

	// time
	GLfloat firstFrame = (GLfloat) glfwGetTime();
	

	glm::vec3 velocity;
	velocity = glm::vec3(1, 0, 0.8f);

	struct Cube 
	{
		glm::vec3 origin = glm::vec3(-2.5f, 0.0f, -2.5f);
		glm::vec3 bound = glm::vec3(2.5f, 5.0f, 2.5f);
	};

	Cube cube;

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		for(int i = 0; i < particleNum; i++)
		{
			// Set frame time
			GLfloat currentFrame = (GLfloat)glfwGetTime() - firstFrame;
			// the animation can be sped up or slowed down by multiplying currentFrame by a factor.
			currentFrame *= 1.5f;
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			/*
			**	INTERACTION
			*/
			// Manage interaction
			app.doMovement(deltaTime);


			/*
			**	SIMULATION
			*/
			glm::vec3 Fg;
			glm::vec3 gravity = glm::vec3(0, -9.8, 0);
			float mass = 5;
			Fg = gravity * mass;

			//set acceleration
			//particles[i].setAcc(gravity * deltaTime);
			glm::vec3 acceleration = (gravity * deltaTime);
			velocity += acceleration;

			for (int i = 0; i < 3; i++)
			{
				if (particles[i].getTranslate()[3][i] < cube.origin[i])
				{
					particles[i].setPos(i, cube.origin[i]);
					velocity[i] *= -0.7f;
				}

				if (particles[i].getTranslate()[3][i] > cube.bound[i])
				{
					particles[i].setPos(i, cube.bound[i]);
					velocity[i] *= -0.7f;
				}
			}

			//update the particle instance using translate and passing the velocity vector
			particles[i].translate(velocity * deltaTime);
		}

		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(plane);
		// draw particles
		for(int i = 0; i < particleNum; i++)
		{
			app.draw(particles[i].getMesh());
		}			

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

