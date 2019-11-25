#include "Plane.h"
#include <SOIL/SOIL.h>

float Plane::randomGen(float min, float max)
{
	/*
	min = min * 1000.0f;
	max = max * 1000.0f;
	std::uniform_int_distribution<> range(min, max);
	float number = range(gen) / 1000.0f;
	//std::cout << "Random number between " << min << " & " << max << " is: " << number << std::endl;
	return number;
	*/
	float random = ((float)rand()) / RAND_MAX;
	float range = max - min;
	return (random * range) + min;
}

Plane::Plane(float randMax, float soft, std::string texName, int type)
{
	this->texName = texName;
	this->stepSize = MapSize - 1;
	this->softness = soft;
	this->randMax = randMax;

	
	initHeight();
	
	if (type == 0) {
		this->loadTexture(Count, this->texName);
		this->textureIDs.push_back(Count);
		Count++;
		this->loadTexture(Count, "mud");
		this->textureIDs.push_back(Count);
		Count++;
		this->loadTexture(Count, "rock");
		this->textureIDs.push_back(Count);
		Count++;
	}
	else {
		this->loadTexture(Count, this->texName);
		this->textureIDs.push_back(Count);
		Count++;
	}
}

Plane::~Plane()
{
}

void Plane::initHeight() {
	for (int x = 0; x < MapSize; x++)
	{
		for (int z = 0; z < MapSize; z++)
		{
			if (x == 0 && z == 0 || x == MapSize - 1 && z == 0 || x == MapSize - 1 && z == MapSize - 1 || x == 0 && z == MapSize - 1) {
				terrain[x][z] = randomGen(-this->randMax, this->randMax);
			}
			else {
				terrain[x][z] = 0;
			}
		}
	}
}

void Plane::loadTexture(int textureID, std::string name)
{
	std::string filePath = "./textures/" + name + ".bmp";
	image = getbmp(filePath);
	//filePath = "./textures/test.png";
	//int width, height;
	//unsigned char* image = SOIL_load_image(filePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	glGenTextures(1, &texture);

	glActiveTexture(GL_TEXTURE0 + textureID);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->sizeX, image->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width,height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	/*grassTexLoc = glGetUniformLocation(programId, "grassTex");
	glUniform1i(grassTexLoc, 0);*/
}

void Plane::calcHeight()
{

	this->stepSize = MapSize - 1;
	while (this->stepSize > 1) {
		for (int x = 0; x < MapSize - 1; x += this->stepSize) {
			for (int y = 0; y < MapSize - 1; y += this->stepSize) {
				Dimondstep(x, y);
			}
		}
		for (int x = 0; x < MapSize - 1; x += this->stepSize) {
			for (int y = 0; y < MapSize - 1; y += this->stepSize) {
				SquareStep(x, y);
			}
		}
		this->randMax = this->randMax * pow(2, -this->softness);
		this->stepSize = this->stepSize / 2;
	}
}

void Plane::Dimondstep(int x, int z)
{
	float average = 0.0f;
	average += terrain[x][z];
	average += terrain[x][z + this->stepSize];
	average += terrain[x + this->stepSize][z];
	average += terrain[x + this->stepSize][z + this->stepSize];
	terrain[x + this->stepSize / 2][z + this->stepSize / 2] = (average / 4) + randomGen(-this->randMax, this->randMax);
}

float Plane::AdvSurroundPoints(int midX, int midZ)
{
	float average = 0.0f;
	int divAmmont = 0;

	if (midZ + this->stepSize / 2 < MapSize && midZ + this->stepSize / 2 >= 0) {
		average += terrain[midX][midZ + this->stepSize / 2];
		divAmmont++;
	}
	if (midZ - this->stepSize / 2 < MapSize && midZ - this->stepSize / 2 >= 0) {
		average += terrain[midX][midZ - this->stepSize / 2];
		divAmmont++;
	}
	if (midX + this->stepSize / 2 < MapSize && midX + this->stepSize / 2 >= 0) {
		average += terrain[midX + this->stepSize / 2][midZ];
		divAmmont++;
	}
	if (midX - this->stepSize / 2 < MapSize && midX - this->stepSize / 2 >= 0) {
		average += terrain[midX - this->stepSize / 2][midZ];
		divAmmont++;
	}

	return (average / divAmmont) + randomGen(-this->randMax, this->randMax);
}

void Plane::SquareStep(int x, int z)
{
	terrain[x][z + this->stepSize / 2] = AdvSurroundPoints(x, z + this->stepSize / 2);
	terrain[x + this->stepSize / 2][z] = AdvSurroundPoints(x + this->stepSize / 2, z);
	terrain[x + this->stepSize][z + this->stepSize / 2] = AdvSurroundPoints(x + this->stepSize, z + this->stepSize / 2);
	terrain[x + this->stepSize / 2][z + this->stepSize] = AdvSurroundPoints(x + this->stepSize / 2, z + this->stepSize);
}

void Plane::initVertexArray() {
	int i = 0;
	float fTextureS = float(MapSize) * 0.1f;
	float fTextureT = float(MapSize) * 0.1f;

	for (int z = 0; z < MapSize; z++)
	{
		for (int x = 0; x < MapSize; x++)
		{
			// Set the coords (1st 4 elements) and a default colour of black (2nd 4 elements) 
			this->terrainVertices[i].coords.x = (float)x;
			this->terrainVertices[i].coords.y = this->terrain[x][z];
			this->terrainVertices[i].coords.z = (float)z;
			this->terrainVertices[i].coords.w = 1.0f;

			this->terrainVertices[i].normals = glm::vec3(0, 0, 0);
			float fScaleC = float(z) / float(MapSize - 1);
			float fScaleR = float(x) / float(MapSize - 1);
			
			this->terrainVertices[i].texcoords = glm::vec2(fTextureS * fScaleC, fTextureT * fScaleR);

			i++;
		}
	}

	i = 0;
	for (int z = 0; z < MapSize - 1; z++)
	{
		i = z * MapSize;
		for (int x = 0; x < MapSize * 2; x += 2)
		{
			this->terrainIndexData[z][x] = i;
			i++;
		}
		for (int x = 1; x < MapSize * 2 + 1; x += 2)
		{
			this->terrainIndexData[z][x] = i;
			i++;
		}
	}
}

void Plane::normalCalc()
{
	//for each point get three indexs
	//then cross between these indexs 
	//then if the x index is odd do the cross the other way around 
	//then if the normal is less than a certain ammount flip it

	for (int z = 0; z < MapSize - 1; z++)
	{
		for (int x = 0; x < MapSize * 2 - 2; x++) {

			int index1 = this->terrainIndexData[z][x];
			int index2 = this->terrainIndexData[z][x + 1];
			int index3 = this->terrainIndexData[z][x + 2];

			glm::vec3 point1;
			glm::vec3 point2;
			glm::vec3 point3;

			point1.x = this->terrainVertices[index1].coords.x;
			point1.y = this->terrainVertices[index1].coords.y;
			point1.z = this->terrainVertices[index1].coords.z;

			point2.x = this->terrainVertices[index2].coords.x;
			point2.y = this->terrainVertices[index2].coords.y;
			point2.z = this->terrainVertices[index2].coords.z;

			point3.x = this->terrainVertices[index3].coords.x;
			point3.y = this->terrainVertices[index3].coords.y;
			point3.z = this->terrainVertices[index3].coords.z;

			glm::vec3 edge1;
			glm::vec3 edge2;

			edge1 = point2 - point1;
			edge2 = point3 - point1;

			glm::vec3 result;
			if (x % 2 == 1) {
				result = glm::cross(edge2, edge1);
			}
			else {
				result = glm::cross(edge1, edge2);
			}

			if (glm::dot(result, glm::vec3(0, 1, 0)) < 0.00001) {
				result = -result;
			}

			this->terrainVertices[index1].normals = glm::normalize(result + terrainVertices[index1].normals);
			this->terrainVertices[index2].normals = glm::normalize(result + terrainVertices[index2].normals);
			this->terrainVertices[index3].normals = glm::normalize(result + terrainVertices[index3].normals);
		}
	}
}

void Plane::setupShaders()
{
	glGenVertexArrays(1, &vao); 
	glGenBuffers(1, &buffer);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->terrainVertices), this->terrainVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(this->terrainVertices[0]), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(this->terrainVertices[0]), (GLvoid*)sizeof(this->terrainVertices[0].coords));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(this->terrainVertices[0]), (GLvoid*)(sizeof(this->terrainVertices[0].coords) + sizeof(this->terrainVertices[0].normals)));
	glEnableVertexAttribArray(2);
}

void Plane::draw()
{
	glBindVertexArray(vao);
	for (int i = 0; i < MapSize - 1; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, 2 * MapSize, GL_UNSIGNED_INT, this->terrainIndexData[i]);
	}
}


