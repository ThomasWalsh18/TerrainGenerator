#include <iostream>
#include <fstream>
#include <vector>
#include "getbmp.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <time.h>
#include "Tree.h"
#include "Plane.h"
#include "glm/gtx/string_cast.hpp"
#include <SOIL/SOIL.h>


#pragma comment(lib, "glew32.lib") 

using namespace std;
using namespace glm;

struct Matrix4x4
{
	float entries[16];
};

static const Matrix4x4 IDENTITY_MATRIX4x4 =
{
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	}
};
struct Material
{
	vec4 ambRefl;
	vec4 difRefl;
	vec4 specRefl;
	vec4 emitCols;
	float shininess;
};
struct Light {
	vec4 ambCols;
	vec4 difCols;
	vec4 specCols;
	vec4 coords;
};

static const Material terrainFandB = {
	vec4(1.0f, 1.0f, 1.0f, 1.0f),
	vec4(1.0f, 1.0f, 1.0f, 1.0f),
	vec4(1.0f, 1.0f, 1.0f, 1.0f),
	vec4(0.0f, 0.0f, 0.0f, 1.0f),
	50.0f
};
 
static const Light light0 = {
	vec4(0.0f, 0.0f, 0.0f, 1.0f),
	vec4(1.0f, 1.0f, 1.0f, 1.0f),
	vec4(1.0f, 1.0f, 1.0f, 1.0f),
	vec4(1.0f, 10.0f, 0.0f, 0.0f)
};

float skyboxVertices[] = {
	// positions for sky box verticeis          
	-1.0f,  1.0f, -1.0f, 
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

static mat4 projMat = mat4(1.0);
static mat3 normalMat = mat3(1.0);
static mat4 viewMat(1.0f);;
static mat4 modelMat(1.0f);

static const vec4 globAmb = vec4(0.2f, 0.2f, 0.2f, 1.0f);

static unsigned int
programId,
programId2,
vertexShaderId,
fragmentShaderId,
modelMatLoc,
cameraPosLoc,
viewMatLoc,
projMatLoc,
normalMatLoc,
texture[1],
grassTexLoc,
rockTexLoc;

bool debugMode = false;
float speed = 3;
vec3 los = glm::vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
vec3 cameraPos = glm::vec3(60.0f, 20.0f, 51.0f);
vec3 cameraFront = glm::vec3(0.0f, -0.45f, 0.89f);

bool firstMouse = true;
float Yaw = 90.0f;	
float Pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float deltaTime = 0.0f;

int oldTimeSinceStart;
int newTimeSinceStart;
vector<Plane*> terrains;
vector<Tree*> trees;

vector<std::string> Skyfilenames;
unsigned int skyboxVAO, skyboxVBO, m_texture;


float random(float min, float max) {
	float random = ((float)rand()) / RAND_MAX;
	float range = max - min;
	return (random * range) + min;
}

void mouseMove(int x, int y)
{
	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	float xoffset = x - lastX;
	float yoffset = lastY - y;
	lastX = x;
	lastY = y;

	float sensitivity = 0.3f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (Pitch > 89.0f) {
		Pitch = 89.0f;
	}
	if (Pitch < -89.0f) {
		Pitch = -89.0f;
	}

	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraFront = glm::normalize(front);
	//force mouse to stay inside the window
	int win_w = glutGet(GLUT_WINDOW_WIDTH);
	int win_h = glutGet(GLUT_WINDOW_HEIGHT);
	if (x < 100 || x > win_w - 100) {  //you can use values other than 100 for the screen edges if you like, kind of seems to depend on your mouse sensitivity for what ends up working best
		lastX = win_w / 2;   //centers the last known position, this way there isn't an odd jump with your cam as it resets
		lastY = win_h / 2;
		glutWarpPointer(win_w / 2, win_h / 2);  //centers the cursor
	}
	else if (y < 100 || y > win_h - 100) {
		lastX = win_w / 2;
		lastY = win_h / 2;
		glutWarpPointer(win_w / 2, win_h / 2);
	}
	if (debugMode) {
		std::cout << "Camera pos :: " << glm::to_string(cameraPos) << std::endl;
		std::cout << "cameraFront :: " << glm::to_string(cameraFront) << std::endl;
	}
}

// Function to read text file, used to read shader files
char* readTextFile(char* aTextFile)
{
	FILE* filePointer = fopen(aTextFile, "rb");
	char* content = NULL;
	long numVal = 0;

	fseek(filePointer, 0L, SEEK_END);
	numVal = ftell(filePointer);
	fseek(filePointer, 0L, SEEK_SET);
	content = (char*)malloc((numVal + 1) * sizeof(char));
	fread(content, 1, numVal, filePointer);
	content[numVal] = '\0';
	fclose(filePointer);
	return content;
}

void shaderCompileTest(GLuint shader)
{
	GLint result = GL_FALSE;
	int logLength;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(shader, logLength, NULL, &vertShaderError[0]);
	cout << &vertShaderError[0] << endl;
}

unsigned int loadCubemap(vector<std::string> faces)
{
	//BitMapFile* Skyimage;
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	unsigned char* Skyimage;
	int width, height;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		std::string filePath = "./textures/skybox/" + faces[i] + ".jpg";
		Skyimage = SOIL_load_image(filePath.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		//Skyimage = getbmp(filePath);
		if (Skyimage != 0)
		{
			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				//0, GL_RGBA, Skyimage->sizeX, Skyimage->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, Skyimage->data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Skyimage);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

// Initialization routine.
void setup(void)
{
	srand(time(NULL));
	Plane* terrain = new Plane(30, 1.5f, "grass", 0);
	Plane* water = new Plane(2, 1, "water", 1);
	terrains.push_back(terrain);
	terrains.push_back(water);
	for (int i = 0; i < terrains.size(); i++) {
		terrains[i]->calcHeight();
		terrains[i]->initVertexArray();
		terrains[i]->normalCalc();
	}

	float percentage = 70.666f;
	float randomNum;
	for (int i = 0; i < MapSize * MapSize; i++){
		randomNum = random(0, 100);
		if (terrains[0]->terrainVertices[i].coords.y > 0.1f){
			if (terrains[0]->terrainVertices[i].coords.y >= 5.0f && terrains[0]->terrainVertices[i].coords.y <= 8.0f) {
				percentage = 88.999f;
			}
			else if (terrains[0]->terrainVertices[i].coords.y > 8.0f) {
				percentage = 99.999f;
			}
			if (randomNum <= percentage) {
			}
			else {
				Tree* temp = new Tree(glm::vec3(terrains[0]->terrainVertices[i].coords.x, terrains[0]->terrainVertices[i].coords.y, terrains[0]->terrainVertices[i].coords.z));
				trees.push_back(temp);
			}
		}
	}
	for (int i = 0; i < trees.size(); i++) {
		trees[i]->createTree();
		trees[i]->buildIndex();
		for (int j = 0; j < trees[i]->leaves.size(); j++) {
			trees[i]->leaves[j]->buildIndex();
		}
	}
	// A flat background colour
	//glClearColor(0.1, 0.4, 1.0, 0.0);
	
	Skyfilenames.push_back("front");
	Skyfilenames.push_back("back");
	Skyfilenames.push_back("up");
	Skyfilenames.push_back("down");
	Skyfilenames.push_back("right");
	Skyfilenames.push_back("left");
	
	m_texture = loadCubemap(Skyfilenames);
	// Create shader program executable - read, compile and link shaders
	char* vertexShader = readTextFile("vertexShader.glsl");
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, (const char**)&vertexShader, NULL);
	glCompileShader(vertexShaderId);
	
	std::cout << "Vertex:: " << std::endl;
	shaderCompileTest(vertexShaderId);

	char* fragmentShader = readTextFile("fragmentShader.glsl");
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, (const char**)&fragmentShader, NULL);
	glCompileShader(fragmentShaderId);

	std::cout << "Fragment:: " << std::endl;
	shaderCompileTest(fragmentShaderId);

	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glUseProgram(programId);
	////////////////////////


	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.ambRefl"), 1, &terrainFandB.ambRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.difRefl"), 1, &terrainFandB.difRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.specRefl"), 1, &terrainFandB.specRefl[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.emitCols"), 1, &terrainFandB.emitCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "terrainFandB.shininess"), 1, &terrainFandB.shininess);
	//////////////////////////
	glUniform4fv(glGetUniformLocation(programId, "globAmb"), 1, &globAmb[0]);
	//////////////////////////
	glUniform4fv(glGetUniformLocation(programId, "light0.ambCols"), 1, &light0.ambCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.difCols"), 1, &light0.difCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.specCols"), 1, &light0.specCols[0]);
	glUniform4fv(glGetUniformLocation(programId, "light0.coords"), 1, &light0.coords[0]);
	/////////////////////////
	
	for (int i = 0; i < terrains.size(); i++) {
		terrains[i]->setupShaders();
	}
	for (int i = 0; i < trees.size(); i++) {
		trees[i]->setupShaders();
		for (int j = 0; j < trees[i]->leaves.size(); j++) {
			trees[i]->leaves[j]->setupShaders();
		}
	}
	
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);

	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(float) * 0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	/////////////////////////
	// Obtain projection matrix uniform location and set value.
	projMatLoc = glGetUniformLocation(programId, "projMat");
	projMat = perspective(radians(60.0), (double) SCREEN_WIDTH / (double)SCREEN_HEIGHT, 0.1, 1000.0);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));
	//////////////////////////////////////
	
	viewMat = glm::mat4(1.0f);
	viewMatLoc = glGetUniformLocation(programId, "viewMat");

	mat4 modelMat = mat4(1.0);
	modelMatLoc = glGetUniformLocation(programId, "modelMat");
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, value_ptr(modelMat));

	normalMatLoc = glGetUniformLocation(programId, "normalMat");
	normalMat = transpose(inverse(mat3(modelMat)));
	glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));	

	cameraPosLoc = glGetUniformLocation(programId, "viewPos");
	
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_POLYGON_SMOOTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	
	viewMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

	if (debugMode == true) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// For each row - draw the triangle strip
	int type = 0;
	for (int i = 0; i < terrains.size(); i++) {
		glUniform1i(glGetUniformLocation(programId, "type"), type);
		glUniform1f(glGetUniformLocation(programId, "time"), deltaTime);
		if (type == 0) {
			glUniform1i(glGetUniformLocation(programId, "Tex"), terrains[i]->textureIDs[0]);
			glUniform1i(glGetUniformLocation(programId, "mudTex"), terrains[i]->textureIDs[1]);
			glUniform1i(glGetUniformLocation(programId, "rockTex"), terrains[i]->textureIDs[2]);
		}
		else if (type == 1) {
			glUniform1i(glGetUniformLocation(programId, "Tex"), terrains[i]->textureIDs.back());

		}
		terrains[i]->draw();
		type++;
	}
	glDisable(GL_CULL_FACE); // Disable backface cull for leaves so they can be seen from all angles
	for (int i = 0; i < trees.size(); i++) {
		type = 3;
		glUniform1i(glGetUniformLocation(programId, "type"), type);
		trees[i]->draw();
		type = 5;
		glUniform1i(glGetUniformLocation(programId, "type"), type);
		for (int j = 0; j < trees[i]->leaves.size(); j++) {
			trees[i]->leaves[j]->draw();
		}
	}


	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	type = 4;
	glUniform1i(glGetUniformLocation(programId, "type"), type);
	glUniform3f(glGetUniformLocation(programId, "campos"), cameraPos.x, cameraPos.y, cameraPos.z);


	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0 + 5); 
	glUniform1i(glGetUniformLocation(programId, "skybox"), m_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
	glutSwapBuffers();
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
}

void idle() {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	if (deltaTime > 360.0f) {
		deltaTime = 0.0f;
	}
	deltaTime += 0.01;
	glutPostRedisplay();
}

void keyInput(unsigned char key, int x, int y)
{
	float cameraSpeed = 1;
	switch (key)
	{
	case 'w':
		cameraPos += cameraSpeed * cameraFront;
		break;
	case 'a':
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		break;
	case 's':
		cameraPos -= cameraSpeed * cameraFront;
		break;
	case 'd':
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		break;
	case 'c':
		debugMode = !debugMode;
		break;	
	case 't':
		// top down
		cameraPos = glm:: vec3(59.0f, 121.0f, 66.0f);
		cameraFront = vec3(-0.017450, -0.999848, -0.000274);
		break;
	case 'y': 
		// side view
		cameraPos = vec3(-2.865140, 40.813656, -6.744401);
		cameraFront = vec3(0.558701, -0.526956, 0.640446);
		break;	
	case 'u':
		// reset view
		cameraPos = glm::vec3(60.0f, 20.0f, 51.0f);
		cameraFront = glm::vec3(0.0f, -0.45f, 0.89f);
		break;	
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("TerrainGeneration");

	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
	glutPassiveMotionFunc(mouseMove);
	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_DEPTH_TEST);        
	glutKeyboardFunc(keyInput);

	setup();

	glutMainLoop();
}
