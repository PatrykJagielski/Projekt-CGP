#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "stb_image.h"

GLuint programTextureNorm;
GLuint textureEarth;
GLuint textureEarthNormal;

bool hittedNoram = false;
bool hittedRef = false;

/*GLuint programColor;
GLuint programTexture;
GLuint programSkybox;
GLuint programDepth;
GLuint programShadow;*/
//GLuint programTextureBasic;

std::vector<float> tangent(1203);

// 4 skybox img
#define STB_IMAGE_IMPLEMENTATION

#define SIZE 30
// skybox

// Sky box
GLuint CubemapTexture;
GLuint programSkybox;
GLuint SkyboxVertexBuffer, SkyboxVertexAttributes;

GLuint textureAsteroid;

GLuint programColor;
GLuint programTexture;
GLuint programReflect;

Core::Shader_Loader shaderLoader;

float appLoadingTime;

obj::Model shipModel;
obj::Model sphereModel;

float cameraAngle = 0;
float cameraAngle2 = 0;
glm::vec3 cameraPos = glm::vec3(-5.0f, 0.0f, 0.0f);
glm::vec3 cameraDir;

float roll = 0;
float pitch = 0.25;
float yaw = 0;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));

std::vector<glm::vec3> planety;

struct Particle
{
	glm::vec3 pos;
	glm::vec3 vel;
	glm::quat rot;
};

struct Bullet
{
	glm::vec3 pos;
	glm::vec3 dir;
};

std::vector<Particle> spaceships;
std::vector<Bullet> pociski;

Particle x;
Bullet pocisk;

static const int NUM_CAMERA_POINTS = 10;
glm::vec3 cameraKeyPoints[NUM_CAMERA_POINTS];

glm::vec3 rom;
glm::vec3 wektorStyczny;

float find_distance(glm::vec3 A, glm::vec3 B) {
	float dx, dy, dz, d;
	dx = A.x - B.x;
	dy = A.y - B.y;
	dz = A.z - B.z;
	d = pow(dx, 2) + pow(dy, 2) + pow(dz, 2);
	return sqrt(d);
}

void fire()
{
	pocisk.pos = cameraPos;
	pocisk.dir = cameraDir;
	pociski.push_back(pocisk);
	
}

const float cubemapVertices[] = {
	// positions
	-SIZE,  SIZE, -SIZE,
	-SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,

	-SIZE, -SIZE,  SIZE,
	-SIZE, -SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE,  SIZE,
	-SIZE, -SIZE,  SIZE,

	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,

	-SIZE, -SIZE,  SIZE,
	-SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE, -SIZE,  SIZE,
	-SIZE, -SIZE,  SIZE,

	-SIZE,  SIZE, -SIZE,
	SIZE,  SIZE, -SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	-SIZE,  SIZE,  SIZE,
	-SIZE,  SIZE, -SIZE,

	-SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE,  SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE,  SIZE,
	SIZE, -SIZE,  SIZE
};

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch(key)
	{
	/*case 'i': roll = roll + 0.11f; break;
	case 'o': pitch = pitch + 0.11f; break;
	case 'p': yaw = yaw + 0.11f; break;
	case 'j': roll = roll - 0.11f; break;
	case 'k': pitch = pitch - 0.11f; break;
	case 'l': yaw = yaw - 0.11f; break;*/
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'f':  fire(); break;
	}
}

int deltax = 0;
int deltay = 0;
void mouse(int x, int y)
{
	/*
	glutWarpPointer(300, 300);
	int mspeed = 500;
	x = x - 300;
	y = y - 300;
	float xf = float(x) / mspeed;
	float yf = float(y) / mspeed;
	pitch = yf;
	roll = xf;
	pitch = pitch;
	yaw = yaw;*/

	glutWarpPointer(300, 300);
	int mspeed = 500;
	deltax += x - 300;
	deltay += y - 300;
	float xf = float(deltax) / mspeed;
	float yf = float(deltay) / mspeed;
	pitch = yf;
	roll = xf;
	pitch = pitch;
	yaw = yaw;
	//std::cout << "x="<< x << "\n";
	//std::cout << "y=" << y << "\n";
}

glm::mat4 createCameraMatrix()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - appLoadingTime;
	int sekunda = int(floorf(time));
	float ulamek = time - sekunda;
	wektorStyczny = glm::normalize(glm::catmullRom(cameraKeyPoints[(sekunda - 1) % NUM_CAMERA_POINTS], cameraKeyPoints[(sekunda) % NUM_CAMERA_POINTS], cameraKeyPoints[(sekunda + 1) % NUM_CAMERA_POINTS], cameraKeyPoints[(sekunda + 2) % NUM_CAMERA_POINTS], ulamek + 0.001) - glm::catmullRom(cameraKeyPoints[(sekunda - 1) % NUM_CAMERA_POINTS], cameraKeyPoints[(sekunda) % NUM_CAMERA_POINTS], cameraKeyPoints[(sekunda + 1) % NUM_CAMERA_POINTS], cameraKeyPoints[(sekunda + 2) % NUM_CAMERA_POINTS], ulamek - 0.001));
	rom = glm::catmullRom(cameraKeyPoints[(sekunda - 1) % NUM_CAMERA_POINTS], cameraKeyPoints[(sekunda) % NUM_CAMERA_POINTS], cameraKeyPoints[(sekunda + 1) % NUM_CAMERA_POINTS], cameraKeyPoints[(sekunda + 2) % NUM_CAMERA_POINTS], ulamek);
	

	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0,1,0);
	cameraAngle2 = atan2f(wektorStyczny.z, wektorStyczny.x);
	//return Core::createViewMatrix(cameraPos, cameraDir, up);
	return Core::createViewMatrix(cameraPos, cameraDir, up, roll, pitch, yaw);
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectReflection(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId) {
	GLuint program = programReflect;
	glUseProgram(program);
	Core::SetActiveTexture(CubemapTexture, "skybox", program, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);


	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);

	Core::DrawModel(model);


	glUseProgram(0);
}

void drawObjectTextureNormal(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId, GLuint normalMap)
{
	GLuint program = programTextureNorm;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(normalMap, "normalMap", program, 1);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	//glm::mat4 lightTransformation = projMatrix * lightMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(program, "lightMatrix"), 1, GL_FALSE, (float*)&lightTransformation);

	Core::DrawModelNormal(model);

	glUseProgram(0);
}


void renderSkybox() {
	glUseProgram(programSkybox);
	glm::mat4 view = glm::mat4(glm::mat3(cameraMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "view"), 1, GL_FALSE, (float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "perspective"), 1, GL_FALSE, (float*)&perspectiveMatrix);

	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(SkyboxVertexAttributes);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	glUseProgram(0);
}

float d = 0;
void renderScene()
{
	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	// Macierz statku "przyczepia" go do kamery. Warto przeanalizowac te linijke i zrozumiec jak to dziala.
	//glm::mat4 shipModelMatrix = glm::translate(rom + wektorStyczny * 0.5f + glm::vec3(0, -0.25f, 0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));

	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0,-0.25f,0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.25f));
	drawObjectColor(&shipModel, shipModelMatrix, glm::vec3(0.6f));

	//drawObjectTexture(&sphereModel, glm::translate(glm::vec3(2,0,2)), glm::vec3(0.8f, 0.2f, 0.3f));
	if(!hittedRef) drawObjectReflection(&sphereModel, glm::translate(glm::vec3(2, 0, 2)), textureAsteroid);
	//drawObjectTexture(&sphereModel, glm::translate(glm::vec3(-2,0,-2)), glm::vec3(0.1f, 0.4f, 0.7f));
	if(!hittedNoram) drawObjectTextureNormal(&sphereModel, glm::translate(glm::vec3(-2, 0, -2)), textureEarth, textureEarthNormal);

	renderSkybox();

	
	for (int i = 0; i < spaceships.size(); i++)
	{
		spaceships[i].vel += glm::vec3(0.0f, 0.01f, 0.00001f);
		//spaceships[i].pos += spaceships[i].vel;
		//spaceships[i].pos = rom + spaceships[i].vel;
		//shipModelMatrix = 		
		glm::mat4 shipModelMatrix2 = glm::translate(spaceships[i].pos + rom + spaceships[i].vel + wektorStyczny * 0.5f + glm::vec3(0, -0.25f, 0)) * glm::rotate(-cameraAngle2 + glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.75f));

		//drawObject(...);
		drawObjectTexture(&shipModel, shipModelMatrix2, glm::vec3(0.1f, 0.4f, 0.7f));
		
	}
	//glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0,-0.25f,0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.25f));
	

	//pociski
	for (int i = 0; i < pociski.size(); i++)
	{	
		//printf("%f\n",pociski[i].pos.x);
		pociski[i].pos += pociski[i].dir * 1/20.0f;
		//spaceships[i].pos += spaceships[i].vel;
		//spaceships[i].pos = rom + spaceships[i].vel;
		//shipModelMatrix = 		...
		glm::mat4 shipModelMatrix2 = glm::translate(pociski[i].pos + pociski[i].dir * 0.5f + glm::vec3(0, -0.25f, 0))  * glm::scale(glm::vec3(0.08f));

		//drawObject(...);
		drawObjectTexture(&sphereModel, shipModelMatrix2, glm::vec3(0.1f, 0.4f, 0.7f));

		float d = find_distance(pociski[i].pos, glm::vec3(2, 0, 2));
		if (d < 1.5) {
			printf("hit!");
			hittedRef = true;
		}
		float d2 = find_distance(pociski[i].pos, glm::vec3(-2, 0, -2));
		if (d2 < 1.5) {
			printf("hit!");
			hittedNoram = true;
		}
		//printf("%f\n", d);
		/*
		d = sqrt(pow((2.0f - pociski[i].pos.x), 2.0) + pow((0.0f - pociski[i].pos.y), 2.0) + pow((2.0f - pociski[i].pos.x), 2.0));


		//printf("%f\n", cameraPos.x);
		if(d < 0.013f) {
			printf("%s\n", "hit!");
		}*/
	}
	


	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programTextureNorm = shaderLoader.CreateProgram("shaders/shader_norm.vert", "shaders/shader_norm.frag");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	programSkybox = shaderLoader.CreateProgram("shaders/skybox.vert", "shaders/skybox.frag");
	programReflect = shaderLoader.CreateProgram("shaders/reflection.vert", "shaders/reflection.frag");
	textureAsteroid = Core::LoadTexture("textures/asteroid2.png");
	textureEarth = Core::LoadTexture("textures/earth.png");
	textureEarthNormal = Core::LoadTexture("textures/earth_normalmap.png");

	CubemapTexture = Core::setupCubeMap(
		"textures/skybox/xpos.png",
		"textures/skybox/xneg.png",
		"textures/skybox/ypos.png",
		"textures/skybox/yneg.png",
		"textures/skybox/zpos.png",
		"textures/skybox/zneg.png"
	);


	//Skybox settings
	glGenBuffers(1, &SkyboxVertexBuffer);
	glGenVertexArrays(1, &SkyboxVertexAttributes);
	glBindVertexArray(SkyboxVertexAttributes);
	glBindBuffer(GL_ARRAY_BUFFER, SkyboxVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), cubemapVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	/*x.pos = glm::vec3(0, 0, -2);
	x.vel = glm::vec3(0,0,0);
	spaceships.push_back(x);
	x.pos = glm::vec3(1, 1, -2);
	spaceships.push_back(x);
	x.pos = glm::vec3(2, 2, -2);
	spaceships.push_back(x);*/

	std::vector<float> unitY = { 0.0, 1.0, 0.0 };
	std::vector<float> unitX = { 1.0, 0.0, 0.0 };

	glm::vec3 uY = { 0.0, -1.0, 0.0 };
	glm::vec3 uX = { -1.0, 0.0, 0.0 };

	glm::vec3 tang[1203];

	for (int i = 0; i < sphereModel.normal.size(); i += 3)
	{
		glm::vec3 normal = { sphereModel.normal[i + 0], sphereModel.normal[i + 1], sphereModel.normal[i + 2] };

		if (sphereModel.normal[i + 1] < 0.99 && sphereModel.normal[i + 1] > -0.99) tang[i] = glm::normalize(glm::cross(normal, uY));
		else tang[i] = glm::normalize(glm::cross(normal, uX));

		tangent[i + 0] = tang[i].x;
		tangent[i + 1] = tang[i].y;
		tangent[i + 2] = tang[i].z;
	}

	sphereModel.tangent = tangent;

	x.vel = glm::vec3(0, 0, 0);
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			x.pos = glm::vec3(i, i % 5, -2 - 2 * j);
			spaceships.push_back(x);
		}
	}

	static const float camRadius = 3.55;
	static const float camOffset = 0.6;
	for (int i = 0; i < NUM_CAMERA_POINTS; i++)
	{
		float angle = (float(i))*(2 * glm::pi<float>() / NUM_CAMERA_POINTS);
		float radius = camRadius * (0.95 + glm::linearRand(0.0f, 0.1f));
		cameraKeyPoints[i] = glm::vec3(cosf(angle) + camOffset, 0.0f, sinf(angle)) * radius;
	}

	appLoadingTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(programSkybox);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("OpenGL Projekt CGP");
	glewInit();

	init();
	glutSetCursor(GLUT_CURSOR_NONE);
	
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
