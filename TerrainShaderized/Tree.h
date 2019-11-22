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

class Tree
{
	struct Vertex
	{
		float coords[4];
		float colors[4];
	};

public:
	Tree(glm::vec3 base1);
	~Tree();
	void buildIndex();
	void setupShaders();
	void draw();
	void computeSingleBranch(float angle, float x0, float y0, float x1, float y1, float& x2, float& y2);
	void recurComputeBranch(int depth, int index, float angle, std::vector<glm::vec2> BasePts, std::vector<glm::vec2> BrPts);
	void createTree();
	float random(float min, float max);

	unsigned int vao, buffer;
	int MAXLEVEL = 0;// = random(4, 7); //maximum tree level
	std::vector<int> branchIndexDataVector;
	Vertex TrunkVertices[NUMPOINTS] = {};
	Vertex LeafVertices[NUMPOINTS] = {};
	glm::vec3 base1;
};

