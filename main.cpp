#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader_class.h"
#include "particle_class.h"

// standard C++ headers 
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>

// screen settings
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// timing 
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// functions
//void drawCircle(unsigned int& VBO, unsigned int& VAO, std::vector<float> position, 
//	float radius, int numberOfSides, Shader& ourShader);
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); /// tell GLFW to use callback on every window resize
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // lock mouse to window

	// Shader object
	Shader ourShader("shader.vs", "shader.fs");

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO); // create OpenGL obj of vertex array object
	glGenBuffers(1, &VBO);      // "                 " vertex buffer object

	// Wireframe mode
 /*   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/

	// Screen scaler
	float scale = 10.0f;

	std::vector<float> position = { 0.0f, 0.0f , 0.0f};
	std::vector<float> velocity = { 0.0f, 0.0f , 0.0f};
	float radius = 50.0f;
	int numSides = 50;

	float dampingFactor = 0.85f;

	Particle particle(position, radius, numSides, ourShader, SCREEN_WIDTH, SCREEN_HEIGHT);

	// RENDERING LOOP
	while (!glfwWindowShouldClose(window)) {

		// Time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (deltaTime > 0.05f) { deltaTime = 0.05f; }

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		particle.drawCircle(VAO, VBO);
		//// Draw particle 
		//drawCircle(VBO, VAO, position, radius, numSides, ourShader);

		//// Update position
		//position[0] += velocity[0] * deltaTime;
		//position[1] += velocity[1] * deltaTime;
		//velocity[1] += (-9.81f) * deltaTime; // scale velocity for screen coord {-1,-1}

		//// Add border collision
		//if (position[1] - (radius / SCREEN_HEIGHT) <= -1.0) // check bottom of screen 
		//{
		//	position[1] = -1.0 + (radius / SCREEN_HEIGHT); // prevent bounce from below floor
		//	velocity[1] *= -1.0 * dampingFactor;

		//	if (std::abs(velocity[1]) < 0.1f) {
		//		velocity[1] = 0.0f;
		//	}
		//}

		glfwSwapBuffers(window);
		glfwPollEvents(); // check for event triggering
	}

	glfwTerminate();
	return 0;
}

//// Draw circle
//void drawCircle(unsigned int& VBO, unsigned int& VAO, std::vector<float> position, 
//	float radius, int numSides, Shader& ourShader) 
//{
//	int numVertices = numSides + 2; 
//
//	float doublePi = 2.0f * 3.141592653f;
//	
//	// Setup verticies vector
//	std::vector<float> vertices;
//	vertices.push_back(position[0]); // x
//	vertices.push_back(position[1]); // y
//	vertices.push_back(position[2]); // z
//	// Trigonometry to calculate vertices (rememeber 0,0 is centre) 
//	for (int i = 1; i < numVertices; i++) {
//		vertices.push_back(position[0] + (radius * cos(i * doublePi / numSides)) / SCREEN_WIDTH); // remember that screen dimensions are normalised
//		vertices.push_back(position[1] + (radius * sin(i * doublePi / numSides)) / SCREEN_HEIGHT); // verticies must be {-1, 1}
//		vertices.push_back(position[2]);
//	}
//
//	glBindVertexArray(VAO);
//
//	// Bind VBO buffer
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); // copy vertex data into buffer
//
//	// Set vertex attribute pointers 
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//	glEnableVertexAttribArray(0);
//
//	// Draw objects 
//	ourShader.use(); // activate shader
//	glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
//	glBindVertexArray(0);
//
//}

void updatePosition(std::vector<float>position, std::vector<float>velocity, float deltaTime) 
{

}

// Whenever window sized changes, callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Input control
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
		glfwSetWindowShouldClose(window, true);
}