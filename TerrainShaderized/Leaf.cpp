#include "Leaf.h"




Leaf::Leaf(glm::vec3 base1)
{
	this->base1 = base1;
	this->base2 = base1 + glm::vec3(1.0f, 1.0f, 0.0f);
	this->base3 = base1 + glm::vec3(-1.0f, 1.0f, 0.0f);
	this->base4 = base1 + glm::vec3(0.0f, 2.0f, 0.0f);

	this->LeafVertices[0].coords[0] = base1.x;
	this->LeafVertices[0].coords[1] = base1.y;
	this->LeafVertices[0].coords[2] = base1.z;
	this->LeafVertices[0].coords[3] = 1.0f;

	this->LeafVertices[1].coords[0] = base2.x;
	this->LeafVertices[1].coords[1] = base2.y + 1.0f;
	this->LeafVertices[1].coords[2] = base2.z;
	this->LeafVertices[1].coords[3] = 1.0f;

	this->LeafVertices[2].coords[0] = base3.x - 1.0f;
	this->LeafVertices[2].coords[1] = base3.y + 1.0f;
	this->LeafVertices[2].coords[2] = base3.z;
	this->LeafVertices[2].coords[3] = 1.0f;

	this->LeafVertices[3].coords[0] = base4.x;
	this->LeafVertices[3].coords[1] = base4.y + 2.0f;
	this->LeafVertices[3].coords[2] = base4.z;
	this->LeafVertices[3].coords[3] = 1.0f;

}
void Leaf::buildIndex()
{


}

Leaf::~Leaf()
{
}

void Leaf::setupShaders()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &buffer);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LeafVertices), LeafVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 0));
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Leaf::draw()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glFlush();
}

void Leaf::createLeaf()
{
}

float Leaf::random(float min, float max)
{
	float random = ((float)rand()) / RAND_MAX;
	float range = max - min;
	return (random * range) + min;
}
