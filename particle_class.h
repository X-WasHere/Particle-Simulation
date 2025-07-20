#ifndef PARTICLE_H
#define PARTICLE_H

#include <glad/glad.h>
#include <vector>

#include "shader_class.h"

class Particle
{
public:
	std::vector<float> position;
	std::vector<float> vertices;
	unsigned int numSides;
	int numVertices;
	float radius;
	
	Shader* ourShader;

	unsigned int SCR_WIDTH;
	unsigned int SCR_HEIGHT;

	// Constructor sets up circle vertices
	Particle(std::vector<float> position_, float radius_, int numSides_, Shader* ourShader_, const unsigned int SCREEN_WIDTH, const unsigned SCREEN_HEIGHT) 
		: position(position_), radius(radius_), numSides(numSides_), ourShader(ourShader_), SCR_WIDTH(SCREEN_WIDTH), SCR_HEIGHT(SCREEN_HEIGHT)
	{
		numVertices = numSides + 2;

		float doublePi = 2.0f * 3.141592653f;

		// Setup verticies vector
		vertices.push_back(position[0]); // x
		vertices.push_back(position[1]); // y
		vertices.push_back(position[2]); // z
		// Trigonometry to calculate vertices (rememeber 0,0 is centre) 
		for (int i = 1; i < numVertices; i++) {
			vertices.push_back(position[0] + (radius * cos(i * doublePi / numSides)) / SCREEN_WIDTH); // remember that screen dimensions are normalised
			vertices.push_back(position[1] + (radius * sin(i * doublePi / numSides)) / SCREEN_HEIGHT); // verticies must be {-1, 1}
			vertices.push_back(position[2]);
		}
	}

	// Member functions 
	void drawCircle(unsigned int& VAO, unsigned int& VBO)
	{
		glBindVertexArray(VAO);

		// Bind VBO buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); // copy vertex data into buffer

		// Set vertex attribute pointers 
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		// Draw objects 
		(*ourShader).use(); // activate shader
		glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
		glBindVertexArray(0);
	}
};

#endif