#pragma once
#include <random>
#include "Contoller.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <time.h>

class Leaf
{
	struct Vertex
	{
		float coords[4];
	};

public:
	Leaf(glm::vec3 base1);
	~Leaf();
	void setupShaders();
	void draw();
	void createLeaf();
	float random(float min, float max);
	void buildIndex();

	unsigned int vao, buffer;

	Vertex LeafVertices[4] = {};
	glm::vec3 base1;
	glm::vec3 base2;
	glm::vec3 base3;
	glm::vec3 base4;
};

