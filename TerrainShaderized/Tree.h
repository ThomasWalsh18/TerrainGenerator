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

#include "Leaf.h"

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
	//void computeSingleBranch(float angle, float x0, float y0, float x1, float y1, float& x2, float& y2);
	void computeSingleBranch(int depth, float angle, float x0, float y0, float z0, float x1, float y1, float z1, float& x2, float& y2, float& z2);
	void recurComputeBranch(int depth, int index, float angle, std::vector<glm::vec3> BasePts, std::vector<glm::vec3> BrPts);
	void createTree();
	float random(float min, float max);
	std::vector<Leaf*> leaves;
	unsigned int vao, buffer;
	int MAXLEVEL = 0;// = random(4, 7); //maximum tree level
	std::vector<int> branchIndexDataVector;
	Vertex TrunkVertices[NUMPOINTS] = {};
	Vertex LeafVertices[NUMPOINTS] = {};
	glm::vec3 base1;
};

