#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader_class.h"
#include "particle.h"

// standard C++ headers 
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <random>

// screen settings
const unsigned int SCREEN_WIDTH = 900;
const unsigned int SCREEN_HEIGHT = 900;

// timing 
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


int main() {
	// GLFW setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window object
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Particles", NULL, NULL);
	if (window == 0)
	{
		std::cout << "Failed to create GLWF window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialzie GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Setup viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // tell GLFW to use callback on every window resize

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Shader object
	Shader ourShader("shader.vs", "shader.fs");
	ourShader.use();
	ourShader.setFloat("opacity", 1.0f);

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO); // create OpenGL obj of vertex array object
	glGenBuffers(1, &VBO);      // vertex buffer object

	float particleRadius = 10.0f;
	int numSides = 100;
	float dampingFactor = 0.85;
	int numParticles = 900;
	ParticleSystem particleSystem(SCREEN_WIDTH, SCREEN_HEIGHT, ourShader, VAO, VBO, particleRadius, numSides, dampingFactor, numParticles, "grid");
	std::vector<Particle> particles = particleSystem.createParticles(); // create array of particle objects


	float smoothingRadius = 1.0f;

	// RENDERING LOOP
	while (!glfwWindowShouldClose(window)) {

		// Time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (deltaTime > 0.05f) { deltaTime = 0.05f; }

		processInput(window);

		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		// Simulation step
		particleSystem.simulationStep(particles, deltaTime);
		particleSystem.drawSystem(particles, ourShader, deltaTime);

		glfwSwapBuffers(window);
		glfwPollEvents(); // check for event triggering
	}

	glfwTerminate();
	return 0;
}


// Whenever window sized changes, callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Input control
void processInput(GLFWwindow* window) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
		glfwSetWindowShouldClose(window, true);
}