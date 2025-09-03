#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <omp.h>

#include "shader_class.h"
#include "utils.h"

class Particle
{
private:
	glm::vec3 position;
	glm::vec3 velocity;
	std::vector<float> vertices;
	unsigned int numSides;
	int numVertices;
	float radius;

	unsigned int SCR_WIDTH;
	unsigned int SCR_HEIGHT;

public:
	// Constructor sets up circle vertices vector with initial xyz position
	Particle(glm::vec3 position_, glm::vec3 velocity_, float radius_, int numSides_, const unsigned int SCREEN_WIDTH, const unsigned int SCREEN_HEIGHT);

	// Member functions 
	void drawCircle(unsigned int& VAO, unsigned int& VBO, Shader& ourShader);
	void add_velocity(float deltaTime);
	void add_border_collision(float dampingFactor);

	friend class ParticleSystem;
};


class ParticleSystem 
{
private: 
	// Particle properties
	int numParticles;
	float particleRadius;
	int numSides;
	float dampingFactor;
	const std::string& arrangement;
	// Create array of positions and velocities 
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> velocities;

	// fluid properties
	std::vector<float> densities;

	// Screen settings and rendering
	unsigned int SCR_WIDTH;
	unsigned int SCR_HEIGHT;
	float deltaTime = 0;

	Shader& ourShader;
	unsigned int& VAO;
	unsigned int& VBO;

	void setDeltaTime(float deltaTime_) { deltaTime = deltaTime_; }

public:
	ParticleSystem(unsigned int SCREEN_WIDTH, unsigned int SCREEN_HEIGHT, Shader& ourShader_, unsigned int& VAO, unsigned int& VBO,
		float particleRadius = 25.0f, int numSides = 50, float dampingFactor = 0.85, int numParticles = 19, const std::string& arrangement = "random");

	// TODO : move to private and add setter functions
	float gasConstant = 0.5f; // pressure multiplier
	float restDensity = 130.0f; // target density 
	float smoothingRadius = 1.0f;
	const float mass = 1.0f; // for simplicity
	

	// Member functions
	std::vector<Particle> createParticles();
	void drawSystem(std::vector<Particle>& particles, Shader& ourShader, float deltaTime);
	float smoothingKernel(float distance);
	float smoothingKernelDerivative(float distance);
	float calculateDensity(glm::vec3 samplePoint);
	float convertDensityToPressure(float density);
	glm::vec3 calculatePressureForce(glm::vec3 samplePoint);
	void updateDensity();
	void simulationStep(float deltaTime, std::vector<Particle>& particles);
};
