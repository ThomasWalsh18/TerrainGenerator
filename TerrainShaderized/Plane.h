#pragma once
#include <iostream>
#include "Contoller.h"
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "getbmp.h"
#include <vector>



class Plane
{
	struct Vertex
	{
		glm::vec4 coords;
		glm::vec3 normals;
		glm::vec2 texcoords;
	};

public:
	BitMapFile* image;
	float randomGen(float min, float max);
	Plane(float randMax, float soft, std::string texName, int type);
	~Plane();
	void initHeight();
	void loadTexture(int textureID, std::string name);
	void calcHeight();
	void Dimondstep(int x, int z);
	void SquareStep(int x, int z);
	void initVertexArray();
	void normalCalc();
	void setupShaders();
	void draw();

	float AdvSurroundPoints(int midX, int midZ);
	Vertex terrainVertices[MapSize * MapSize] = {};
	float terrain[MapSize][MapSize] = {};
	int terrainIndexData[MapSize - 1][2 * MapSize] = {};
	std::string texName;
	int stepSize;
	std::vector<int>textureIDs;
	float softness;
	float randMax;

	unsigned int vao, buffer, texture;

};

