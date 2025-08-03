#pragma once

#include <glad/glad.h>
#include <vector>

#include "shader_class.h"
#include "utils.h"

class Particle
{
//private:
public:
	std::vector<float> position;
	std::vector<float> vertices;
	std::vector<float> velocity;
	unsigned int numSides;
	int numVertices;
	float radius;

	unsigned int SCR_WIDTH;
	unsigned int SCR_HEIGHT;

//public:

	// Constructor sets up circle vertices vector with initial xyz position
	Particle(std::vector<float> position_, std::vector<float> velocity_, float radius_, int numSides_, const unsigned int SCREEN_WIDTH, const unsigned SCREEN_HEIGHT);

	// Member functions 
	void drawCircle(unsigned int& VAO, unsigned int& VBO, Shader& ourShader);
	void add_velocity(float deltaTime);
	void add_border_collision(float dampingFactor);
};


class ParticleSystem 
{
private: 
	// Particle properties
	int numParticles;
	float particleRadius;
	int numSides;
	float dampingFactor;
	// Create array of positions and velocities 
	std::vector<std::vector<float>> positions;
	std::vector<std::vector<float>> velocities;

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
		float particleRadius = 25.0f, int numSides = 50, float dampingFactor = 0.85, int numParticles = 19);

	// Member functions
	std::vector<Particle> createParticles();
	void drawSystem(std::vector<Particle>& particles, Shader& ourShader, float deltaTime);
};
