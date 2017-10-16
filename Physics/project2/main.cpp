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

void BlowDryer() 
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
	int particleNum = 40;
	for (int i = 0; i < particleNum; i++)
	{
		Particle p = Particle::Particle();
		particles.push_back(p);
		//scale it down (x.1), translate it up by 2.5 and rotate it by 90 degrees around the x axis
		//particles[i].setPos(glm::vec3(0.0f, 4.0f, 0.0f));
		particles[i].scale(glm::vec3(0.5f, 0.5f, 0.5f));
		//particles[i].rotate((GLfloat) M_PI_2, glm::vec3(0.0f, 1.0f, 0.0f));
		particles[i].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));


		//initial velocty
		//particles[i].setVel(glm::vec3(sin(i), 0.0f, cos(i)));

		//make ring
		particles[i].setPos(glm::vec3(sin(i), 3.0f, cos(i)));
	}

	//height marker particle
	Particle m = Particle::Particle();
	m.setPos(glm::vec3(0, 3, 0));
	m.scale(glm::vec3(0.5f, 0.5f, 0.5f));
	m.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));


	// time
	GLfloat firstFrame = (GLfloat)glfwGetTime();

	//fixed timestep
	double physicsTime = 0.0f;
	const double fixedDeltaTime = 0.01f;
	double currentTime = (GLfloat)glfwGetTime();
	double accumulator = 0.0f;

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

		//fixed timstep
		double newTime = (GLfloat)glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= fixedDeltaTime)
		{
			for (int i = 0; i < particleNum; i++)
			{
				/*
				**	SIMULATION
				*/
				/*glm::vec3 Fg;
				float mass = 5;
				Fg = gravity * mass;*/


				//set acceleration
				particles[i].setAcc(gravity);

				//Semi-Implicit Euler integration
				particles[i].getVel() += particles[i].getAcc() * fixedDeltaTime;
				particles[i].setPos(particles[i].getPos() + particles[i].getVel() * fixedDeltaTime);

				//collisions to bound within the box
				for (int j = 0; j < 3; j++)
				{
					if (particles[i].getTranslate()[3][j] < cube.origin[j])
					{
						glm::vec3 diff = glm::vec3(0.0f);
						diff[j] = cube.origin[j] - particles[i].getPos()[j];
						particles[i].setPos(j, cube.origin[j] + diff[j]);
						particles[i].getVel()[j] *= -1.0f;
					}

					if (particles[i].getTranslate()[3][j] > cube.bound[j])
					{
						glm::vec3 diff = glm::vec3(0.0f);
						diff[j] = cube.origin[j] - particles[i].getPos()[j];
						particles[i].setPos(j, cube.bound[j] + diff[j]);
						particles[i].getVel()[j] *= -1.0f;
					}

				}
			}

			accumulator -= fixedDeltaTime;
			physicsTime += fixedDeltaTime;
		}

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

		//render height marker
		app.draw(m.getMesh());

		app.display();
	}

	app.terminate();
}

void RigidbodyRotate()
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
	int particleNum = 40;
	for (int i = 0; i < particleNum; i++)
	{
		Particle p = Particle::Particle();
		particles.push_back(p);
		//scale it down (x.1), translate it up by 2.5 and rotate it by 90 degrees around the x axis
		//particles[i].setPos(glm::vec3(0.0f, 4.0f, 0.0f));
		particles[i].scale(glm::vec3(0.5f, 0.5f, 0.5f));
		//particles[i].rotate((GLfloat) M_PI_2, glm::vec3(0.0f, 1.0f, 0.0f));
		particles[i].getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));


		//initial velocty
		//particles[i].setVel(glm::vec3(sin(i), 0.0f, cos(i)));

		//make ring
		particles[i].setPos(glm::vec3(sin(i), 3.0f, cos(i)));
	}

	//height marker particle
	Particle m = Particle::Particle();
	m.setPos(glm::vec3(0, 3, 0));
	m.scale(glm::vec3(0.5f, 0.5f, 0.5f));
	m.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));


	// time
	GLfloat firstFrame = (GLfloat)glfwGetTime();

	//fixed timestep
	double physicsTime = 0.0f;
	const double fixedDeltaTime = 0.01f;
	double currentTime = (GLfloat)glfwGetTime();
	double accumulator = 0.0f;

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

		//fixed timstep
		double newTime = (GLfloat)glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= fixedDeltaTime)
		{
			for (int i = 0; i < particleNum; i++)
			{
				/*
				**	SIMULATION
				*/
				/*glm::vec3 Fg;
				float mass = 5;
				Fg = gravity * mass;*/


				//set acceleration
				particles[i].setAcc(gravity);

				//Semi-Implicit Euler integration
				particles[i].getVel() += particles[i].getAcc() * fixedDeltaTime;
				particles[i].setPos(particles[i].getPos() + particles[i].getVel() * fixedDeltaTime);

				//collisions to bound within the box
				for (int j = 0; j < 3; j++)
				{
					if (particles[i].getTranslate()[3][j] < cube.origin[j])
					{
						glm::vec3 diff = glm::vec3(0.0f);
						diff[j] = cube.origin[j] - particles[i].getPos()[j];
						particles[i].setPos(j, cube.origin[j] + diff[j]);
						particles[i].getVel()[j] *= -1.0f;
					}

					if (particles[i].getTranslate()[3][j] > cube.bound[j])
					{
						glm::vec3 diff = glm::vec3(0.0f);
						diff[j] = cube.origin[j] - particles[i].getPos()[j];
						particles[i].setPos(j, cube.bound[j] + diff[j]);
						particles[i].getVel()[j] *= -1.0f;
					}

				}
			}

			accumulator -= fixedDeltaTime;
			physicsTime += fixedDeltaTime;
		}

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

		//render height marker
		app.draw(m.getMesh());

		app.display();
	}

	app.terminate();

	
}


// main function
int main()
{
	//BlowDryer
	RigidbodyRotate();
	
	return EXIT_SUCCESS;
}

