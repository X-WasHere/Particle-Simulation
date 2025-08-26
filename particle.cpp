#include "particle.h"

// ------------------ class Particle ------------------
// Constructor sets up circle vertices vector with initial xyz position
Particle::Particle(glm::vec3 position_, glm::vec3 velocity_, float radius_, int numSides_, const unsigned int SCREEN_WIDTH, const unsigned SCREEN_HEIGHT)
	: position(position_), velocity(velocity_), radius(radius_), numSides(numSides_), SCR_WIDTH(SCREEN_WIDTH), SCR_HEIGHT(SCREEN_HEIGHT)
{
	numVertices = numSides + 2;

	// Setup verticies vector
	vertices.push_back(position.x); // x
	vertices.push_back(position.y); // y
	vertices.push_back(position.z); // z
}

// Member functions 
void Particle::drawCircle(unsigned int& VAO, unsigned int& VBO, Shader& ourShader)
{
	float doublePi = 2.0f * 3.141592653f;
	vertices.clear();
	// Trigonometry to calculate vertices (rememeber 0,0 is centre) 
	for (int i = 1; i < numVertices; i++) {
		vertices.push_back(position.x + (radius * cos(i * doublePi / numSides)) / SCR_WIDTH); // remember that screen dimensions are normalised
		vertices.push_back(position.y + (radius * sin(i * doublePi / numSides)) / SCR_HEIGHT); // verticies must be {-1, 1}
		vertices.push_back(position.z);
	}

	glBindVertexArray(VAO);

	// Bind VBO buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); // copy vertex data into buffer

	// Set vertex attribute pointers 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Draw objects 
	ourShader.use(); // activate shader
	glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
	glBindVertexArray(0);
}

void Particle::add_velocity(float deltaTime)
{
	// Update position with velocity verlet
	position.x += velocity.x * deltaTime;
	position.y += velocity.y * deltaTime + 0.5f * (-9.81f) * deltaTime * deltaTime;
	velocity.y += (-9.81f) * deltaTime;
}

void Particle::add_border_collision(float dampingFactor)
{
	// vertical collision
	if (position.y - (radius / SCR_HEIGHT) <= -1.0f) // check bottom of screen 
	{
		position.y = -1.0f + (radius / SCR_HEIGHT); // prevent bounce from below floor
		velocity.y *= -1.0f * dampingFactor;

		if (std::abs(velocity.y) < 0.2f) { // prevent "infinite" bounce due to gravity
			velocity.y = 0.0f;
		}
	}
	if (position.y + (radius / SCR_HEIGHT) >= 1.0) // check top of screen 
	{
		position.y = 1.0f - (radius / SCR_HEIGHT); // prevent bounce from above ceiling
		velocity.y *= -1.0f * dampingFactor;
	}

	// horizontal collision
	if (position.x - (radius / SCR_WIDTH) <= -1.0f) // check left side of screen 
	{
		position.x = -1.0f + (radius / SCR_WIDTH);
		velocity.x *= -1.0f * dampingFactor;
	}
	if (position.x + (radius / SCR_WIDTH) >= 1.0f) // check right side of screen 
	{
		position.x = 1.0f - (radius / SCR_WIDTH);
		velocity.x *= -1.0f * dampingFactor;
	}
}


// ------------------ class ParticleSystem ------------------
ParticleSystem::ParticleSystem(unsigned int SCREEN_WIDTH, unsigned int SCREEN_HEIGHT, Shader& ourShader, unsigned int& VAO, unsigned int& VBO,
	float particleRadius, int numSides, float dampingFactor, int numParticles, const std::string& arrangement): 
	SCR_WIDTH(SCREEN_WIDTH), SCR_HEIGHT(SCREEN_HEIGHT), ourShader(ourShader), VAO(VAO), VBO(VBO), particleRadius(particleRadius), numSides(numSides), 
	dampingFactor(dampingFactor), numParticles(numParticles), arrangement(arrangement)
{
	glm::vec3 position(0.0f, 0.0f , 0.0f);
	glm::vec3 velocity(0.0f, 0.0f , 0.0f);

	densities.resize(numParticles);

	// Arrange particles
	try {
		if (arrangement == "random") {
			for (int i = 0; i < numParticles; i++) {
				position.x = utils::randFloat(1);
				position.y = utils::randFloat(1);
				velocity.x = utils::randFloat(5);
				velocity.y = 0;
				velocities.push_back(velocity);
				positions.push_back(position);
			}
		}
		else if (arrangement == "grid") {
			float gridDim = sqrt(numParticles);
			float spacing = 4 * particleRadius;
			float offset = ((spacing * gridDim) - 2 * particleRadius) / 2;
			for (int i = 0; i < static_cast<int>(gridDim); i++) {
				for (int j = 0; j < static_cast<int>(gridDim); j++) {
					position.x = (i * spacing / SCR_WIDTH) - (offset / SCR_WIDTH);
					position.y = (j * spacing / SCR_HEIGHT) - (offset / SCR_HEIGHT);
					positions.push_back(position);

					velocity.x = utils::randFloat(5);
					velocity.y = 0;
					velocities.push_back(velocity);
				}
			}
		}
		else {
			throw(arrangement);
		}
	}
	catch (const std::string& arrangement) {
		std::cout << "Invalid arragement: " << arrangement << std::endl;
	}

}

std::vector<Particle> ParticleSystem::createParticles()
{
	std::vector<Particle> particles;
	for (int i = 0; i < positions.size(); i++) {
		Particle particle(positions[i], velocities[i], particleRadius, numSides, SCR_WIDTH, SCR_HEIGHT);
		particles.push_back(particle);
	}

	return particles;
}

void ParticleSystem::drawSystem(std::vector<Particle>& particles, Shader& ourShader, float deltaTime) 
{
	for (Particle& particle : particles) {

		setDeltaTime(deltaTime);
		particle.drawCircle(VAO, VBO, ourShader);
		particle.add_velocity(deltaTime);
		particle.add_border_collision(dampingFactor);
	}
}

float ParticleSystem::SmoothingKernel(float radius, float distance)
{	
	if (distance >= radius) { return 0; }

	float densityFunctionVolume = (3.14159265359 * pow(radius, 4)) / 2;
	float value = radius - distance;
	return value * value * value / densityFunctionVolume;
}

float ParticleSystem::CalculateDensity(glm::vec3 samplePoint, float smoothingRadius)
{
	float density = 0;
	const float mass = 1; // for simplicity

#pragma omp parallel for reduction(+:density)
	for (int i = 0; i < positions.size(); i++) {
		float distance = glm::length(positions[i] - samplePoint); 
		float influence = SmoothingKernel(smoothingRadius, distance);
		density += mass * influence;
	}
	std::cout << "Density is : " << density << std::endl;
	return density;
}

void ParticleSystem::UpdateDensity(float smoothingRadius)
{
	for (int i = 0; i < positions.size(); i++) {
		densities[i] = CalculateDensity(positions[i], smoothingRadius);
	}
}