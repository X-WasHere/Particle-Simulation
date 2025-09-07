#include "particle.h"

// ------------------ class Particle ------------------
// Constructor sets up circle vertices vector with initial xyz position
Particle::Particle(glm::vec3* position_, glm::vec3* velocity_, float radius_, int numSides_, const unsigned int SCREEN_WIDTH, const unsigned int SCREEN_HEIGHT)
	: position(position_), velocity(velocity_), radius(radius_), numSides(numSides_), SCR_WIDTH(SCREEN_WIDTH), SCR_HEIGHT(SCREEN_HEIGHT)
{
	numVertices = numSides + 2;

	// Setup verticies vector
	vertices.push_back(position->x); // x
	vertices.push_back(position->y); // y
	vertices.push_back(position->z); // z
}

// Member functions 
void Particle::drawCircle(unsigned int& VAO, unsigned int& VBO, Shader& ourShader)
{
	float doublePi = 2.0f * 3.141592653f;
	vertices.clear();
	// Trigonometry to calculate vertices (rememeber 0,0 is centre) 
	for (int i = 1; i <= numVertices; i++) {
		vertices.push_back(position->x + (radius * cos(i * doublePi / numSides)) / SCR_WIDTH); // remember that screen dimensions are normalised
		vertices.push_back(position->y + (radius * sin(i * doublePi / numSides)) / SCR_HEIGHT); // verticies must be {-1, 1}
		vertices.push_back(position->z);
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

void Particle::add_gravity(float deltaTime)
{
	// Update position with velocity verlet
	//position->x += velocity->x * deltaTime;
	position->y += velocity->y * deltaTime + 0.5f * (-9.81f) * deltaTime * deltaTime;
	velocity->y += (-9.81f) * deltaTime;
}

void Particle::add_border_collision(float dampingFactor)
{
	// vertical collision
	if (position->y - (radius / SCR_HEIGHT) <= -1.0f) // check bottom of screen 
	{
		position->y = -1.0f + (radius / SCR_HEIGHT); // prevent bounce from below floor
		velocity->y *= -1.0f * dampingFactor;

		if (std::abs(velocity->y) < 0.2f) { // prevent "infinite" bounce due to gravity
			velocity->y = 0.0f;
		}
	}
	if (position->y + (radius / SCR_HEIGHT) >= 1.0) // check top of screen 
	{
		position->y = 1.0f - (radius / SCR_HEIGHT); // prevent bounce from above ceiling
		velocity->y *= -1.0f * dampingFactor;
	}

	// horizontal collision
	if (position->x - (radius / SCR_WIDTH) <= -1.0f) // check left side of screen 
	{
		position->x = -1.0f + (radius / SCR_WIDTH);
		velocity->x *= -1.0f * dampingFactor;
	}
	if (position->x + (radius / SCR_WIDTH) >= 1.0f) // check right side of screen 
	{
		position->x = 1.0f - (radius / SCR_WIDTH);
		velocity->x *= -1.0f * dampingFactor;
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
				//velocity.x = utils::randFloat(5);
				velocity.x = 0;
				velocity.y = 0;
				velocities.push_back(velocity);
				positions.push_back(position);
			}
		}
		else if (arrangement == "grid") {
			float gridDim = sqrt(numParticles);
			float spacing = 4 * particleRadius;
			float offset = (spacing * (gridDim - 1)) / 2;

			for (int i = 0; i < static_cast<int>(gridDim); i++) {
				for (int j = 0; j < static_cast<int>(gridDim); j++) {
					position.x = (i * spacing / SCR_WIDTH) - (offset / SCR_WIDTH);
					position.y = (j * spacing / SCR_HEIGHT) - (offset / SCR_HEIGHT);
					positions.push_back(position);

					/*velocity.x = utils::randFloat(5);*/
					velocity.x = 0;
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
		Particle particle(&positions[i], &velocities[i], particleRadius, numSides, SCR_WIDTH, SCR_HEIGHT);
		particles.push_back(particle);
	}

	return particles;
}

void ParticleSystem::drawSystem(std::vector<Particle>& particles, Shader& ourShader, float deltaTime) 
{
	for (Particle& particle : particles) 
	{
		setDeltaTime(deltaTime);
		particle.drawCircle(VAO, VBO, ourShader);
	}
}

float ParticleSystem::smoothingKernel(float distance)
{	
	if (distance >= smoothingRadius) { return 0; }

	//float densityFunctionVolume = (3.14159265359 * pow(smoothingRadius, 5)) / 10;
	//float f = smoothingRadius - distance;
	//return f * f * f / densityFunctionVolume;

	// Quadratic smoothing kernel
	float volume = (3.14159265359 * pow(smoothingRadius, 4)) / 6;
	return (smoothingRadius - distance) * (smoothingRadius * distance) / volume;
}

float ParticleSystem::smoothingKernelDerivative(float distance)
{
	if (distance >= smoothingRadius) { return 0; }
	
	//float k = -30 / (3.14159265359 * pow(smoothingRadius, 5)); // normalisation
	//float f = smoothingRadius - distance; // kernel function
	//return k * f * f;

	// Quadratic smoothing kernel
	float scale = 12 / (pow(smoothingRadius, 4) * 3.14159265359);
	return (distance - smoothingRadius) * scale;
}

float ParticleSystem::calculateDensity(int particleIndex)
{
	float density = 0;

	#pragma omp parallel for reduction(+:density)
	for (int i = 0; i < positions.size(); i++) {
		if (i == particleIndex) { continue; }
		float distance = glm::length(positions[i] - positions[particleIndex]);
		float influence = smoothingKernel(distance);
		density += mass * influence;
	}
	//std::cout << "Density is : " << density << std::endl;
	return density;
}

float ParticleSystem::convertDensityToPressure(float density)
{
	float densityError = density - restDensity;
	float pressure = gasConstant * densityError;
	return pressure;
}

glm::vec3 ParticleSystem::calculatePressureForce(int particleIndex)
{
	glm::vec3 pressureForce(0.0f, 0.0f, 0.0f);
	glm::vec3 direction(0.0f,0.0f, 0.0f);

	for (int i = 0; i < positions.size(); i++) 
	{
		if (i == particleIndex) { continue; } // particle doesn't exert pressure on itself

		float distance = glm::length(positions[i] - positions[particleIndex]);

		if (distance < 1e-6f) { 
			direction = glm::vec3(utils::randFloat(1), utils::randFloat(1), 0); 
		}
		else {
			direction = ((positions[i] - positions[particleIndex]) / distance);
		}

		float grad = smoothingKernelDerivative(distance);
		float density = densities[i];
		pressureForce += convertDensityToPressure(density) * mass * grad * direction / density; // seems like you get more repulsion if you don't add negative
	}

	return pressureForce;
}

void ParticleSystem::updateDensity()
{
	// calculate density for every particle due to influence of all other particles
	for (int i = 0; i < positions.size(); i++) {
		densities[i] = calculateDensity(i);
	}
}

void ParticleSystem::simulationStep(std::vector<Particle>& particles, float deltaTime)
{
	// Calculate densities
	updateDensity();

	#pragma omp parallel for
	for (int i = 0; i < positions.size(); i++)
	{
		// Calculate and apply pressure forces
		glm::vec3 pressureForce = calculatePressureForce(i);
		//std::cout << glm::to_string(pressureForce) << std::endl;
		glm::vec3 pressureAcceleration = pressureForce / densities[i];
		velocities[i] += pressureAcceleration * deltaTime;

		// Update positions, resolve collisions
		positions[i] += velocities[i] * deltaTime;
		particles[i].add_border_collision(dampingFactor);
		particles[i].add_gravity(deltaTime);
	}
}