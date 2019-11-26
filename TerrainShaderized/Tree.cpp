#include "Tree.h"

float Tree::random(float min, float max) {
	float random = ((float)rand()) / RAND_MAX;
	float range = max - min;
	return (random * range) + min;
}

Tree::Tree(glm::vec3 base1)
{
	this->MAXLEVEL = random(4, 7);
	this->base1 = base1;
}

Tree::~Tree()
{
}
void Tree::buildIndex() {

	branchIndexDataVector.push_back(0);
	branchIndexDataVector.push_back(1);

	int newLeftVertexIndex = 0;
	int newRightVertexIndex = 0;
	int previousVertexIndex = 1;

	for (int i = 0; i < pow(2, this->MAXLEVEL) - 1; i++) {
		branchIndexDataVector.push_back(previousVertexIndex);
		newLeftVertexIndex = previousVertexIndex * 2;
		branchIndexDataVector.push_back(newLeftVertexIndex);

		branchIndexDataVector.push_back(previousVertexIndex);
		newRightVertexIndex = previousVertexIndex * 2 + 1;
		branchIndexDataVector.push_back(newRightVertexIndex);

		previousVertexIndex++;
	}
}
void Tree::draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_LINES, branchIndexDataVector.size(), GL_UNSIGNED_INT, &branchIndexDataVector[0]);

	glFlush();
}

void Tree::setupShaders()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &buffer);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TrunkVertices), TrunkVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TrunkVertices[0]), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TrunkVertices[0]), (GLvoid*)sizeof(TrunkVertices[0].coords));
	glEnableVertexAttribArray(1);
	///////////////////////////////////////
}

void Tree::computeSingleBranch(int depth, float angle, float x0, float y0,float z0, float x1, float y1, float z1, float& x2, float& y2, float &z2)
{
	float R = random(60, 90) / 100;
	float xs, ys, zs, xll, yll, zll, xl, yl, zl, m; 
	double val;

	xs = (x1 - x0) * R; 
	ys = (y1 - y0) * R; 
	zs = (z1 - z0) * R;

	m = sqrt(xs * xs + ys * ys + zs * zs);

	xll = cos(angle / 2.0) * xs - sin(angle / 2.0) * ys;
	yll = sin(angle / 2.0) * xs + cos(angle / 2.0) * ys;
	zll = 0.0f;

	srand(time(NULL)); // get different srand values
	if (depth % 2 == 0) {
		val = -2.0; 
	}
	else {
		val = 2.0;
	}
	xl = cos(val * angle / 2.0) * xll - sin(val * angle / 2.0) * zll;
	yl = yll; 
	zl = sin(val * angle / 2.0) * xll + cos(val * angle / 2.0) * zll;

	x2 = x1 + xl;
	y2 = y1 + yl;
	z2 = z1 + zl;
}
void Tree::recurComputeBranch(int depth, int index, float angle, std::vector<glm::vec3> BasePts, std::vector<glm::vec3> BrPts)
{
	float x2, y2, z2;
	glm::vec3 ttPt;
	std::vector<glm::vec3> NewBasePts, NewBrPts;

	if (depth > this->MAXLEVEL) { // done all the points
		//for (int i = 0; i < BrPts.size(); i++) { // this could be done to create as many leaves as there is branches, however this becomes very laggy
			Leaf* tempLeaf = new Leaf(BrPts[0]);//BrPts[i];
			leaves.push_back(tempLeaf);
		//}
		return;
	}
	else {
		int size = BasePts.size();
		if (size == 0) { // if there are no base points
			return;
		}
		else {
			for (int i = 0; i < size; i++) {
				angle = glm::radians(random(30, 50));
				computeSingleBranch(depth,angle, BasePts[i].x, BasePts[i].y, BasePts[i].z, BrPts[i].x, BrPts[i].y, BrPts[i].z, x2, y2, z2);
				TrunkVertices[index].coords[0] = x2;
				TrunkVertices[index].coords[1] = y2;
				if (depth == 0) {
					TrunkVertices[index].coords[2] = BasePts[i].z;
				}
				else {
					TrunkVertices[index].coords[2] = z2;
				}
				TrunkVertices[index].coords[3] = 1.0;
				index++;
				NewBasePts.push_back(BrPts[i]);
				ttPt.x = x2;
				ttPt.y = y2;
				ttPt.z = z2;
				NewBrPts.push_back(ttPt);

				angle = glm::radians(random(30, 50));
				computeSingleBranch(depth ,-angle, BasePts[i].x, BasePts[i].y, BasePts[i].z, BrPts[i].x, BrPts[i].y, BrPts[i].z, x2, y2, z2);
				TrunkVertices[index].coords[0] = x2;
				TrunkVertices[index].coords[1] = y2;
				if (depth == 0) {
					TrunkVertices[index].coords[2] = BasePts[i].z;
				}
				else {
					TrunkVertices[index].coords[2] = z2;
				}
				TrunkVertices[index].coords[3] = 1.0;
				index++;
				NewBasePts.push_back(BrPts[i]);
				ttPt.x = x2;
				ttPt.y = y2;
				ttPt.z = z2;
				NewBrPts.push_back(ttPt);
			}
			depth++;
			recurComputeBranch(depth, index, angle, NewBasePts, NewBrPts);
		}
	}
}

void Tree::createTree() {
	int index;
	srand(time(NULL));
	float angle = glm::radians(random(30, 50));

	glm::vec3 ttPt;
	std::vector<glm::vec3> BasePts, BranchPts;

	for (int i = 0; i < NUMPOINTS; i++) {
		TrunkVertices[i].colors[0] = 0.55;
		TrunkVertices[i].colors[1] = 0.27;
		TrunkVertices[i].colors[2] = 0.075;
		TrunkVertices[i].colors[3] = 1.0;
	}
	//Making the orignal trunk
	TrunkVertices[0].coords[0] = this->base1.x;
	TrunkVertices[0].coords[1] = this->base1.y;
	TrunkVertices[0].coords[2] = this->base1.z;
	TrunkVertices[0].coords[3] = 1.0;
	ttPt.x = TrunkVertices[0].coords[0];//x0
	ttPt.y = TrunkVertices[0].coords[1]; //y0 
	ttPt.z = TrunkVertices[0].coords[2]; //y0 
	BasePts.push_back(ttPt);

	TrunkVertices[1].coords[0] = this->base1.x;
	TrunkVertices[1].coords[1] = this->base1.y + random(2.0f, 4.0f);
	TrunkVertices[1].coords[2] = this->base1.z;
	TrunkVertices[1].coords[3] = 1.0;
	ttPt.x = TrunkVertices[1].coords[0]; //x1 
	ttPt.y = TrunkVertices[1].coords[1]; //y2  
	ttPt.z = TrunkVertices[1].coords[2]; //y2  
	BranchPts.push_back(ttPt);

	index = 2;
	recurComputeBranch(0, index, angle, BasePts, BranchPts);
}
