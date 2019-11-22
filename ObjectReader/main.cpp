#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Shader.h"
#include "Mesh.h"
#include "ObjectReader.h"
#include "Material.h"
#include "MaterialReader.h"
#include "Object.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

void window_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(vector<Object*>);
void readFiles(vector<Object*>);
void loadCurveLine(const GLchar* path);
void scaleCurve(vector<glm::vec3*>* points, float factor);
void preparePrintingData(vector<Object*>);
float calculateDirection(int inta, int intb);
void draw(vector<Object*>);
Material* getMaterialObject(string materialId);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const float cameraSpeed = 0.005f;
float lastX = 400, lastY = 300;

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 6.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = normalize(cameraPos - cameraTarget);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = normalize(cross(up, cameraDirection));
glm::vec3 cameraUp = cross(cameraDirection, cameraRight);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float previousX = 640.0f / 2.0;
float previousY = 480.0f / 2.0;
float fov = 45.0f;

float scale = 1.001f;
float movement = 0.001f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;

GLFWwindow* window;
vector<Material*> materials;
vector<glm::vec3*>* curvePoints = new vector<glm::vec3*>();

int main() {
	vector<Object*> objects;

	Object* obj1 = new Object("curve.obj");
	objects.push_back(obj1);

	Object* obj2 = new Object("cube.obj");
	objects.push_back(obj2);

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Teste de versão OpenGL", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Error while opening window with GLFW3\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);

	glewExperimental = GL_TRUE;
	glewInit();

	loadCurveLine("originalCurve.txt");
	scaleCurve(curvePoints, 8);

	readFiles(objects);
	preparePrintingData(objects);
	draw(objects);

	glfwTerminate();
	return 0;
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void processInput(vector<Object*> objects) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void readFiles(vector<Object*> objects) {
	for (int i = 0; i < objects.size(); i++) {
		ObjectReader* objectReader = new ObjectReader(objects[i]->getFilePath());
		Mesh* mesh = objectReader->readFile();
		objects[i]->setMesh(mesh);
		MaterialReader* matReader = new MaterialReader(mesh->getMaterialFile());
		vector<Material*> fileMaterials = matReader->readFile();
		for (int m = 0; m < fileMaterials.size(); m++) {
			materials.push_back(fileMaterials[m]);
		}
	}
}

void loadCurveLine(const GLchar* path) {
	ifstream file;

	file.open(path);

	if (!file.is_open()) {
		cout << "Error while reading file.";
	}

	string line, temp;
	stringstream sstream;

	while (!file.eof()) {

		sstream = std::stringstream();
		line = temp = "";

		std::getline(file, line);

		sstream << line;
		sstream >> temp;

		if (temp == "v") {
			float x, y, z;
			sstream >> x >> y >> z;
			curvePoints->push_back(new glm::vec3(x, y, z));
		}
	}
	file.close();
}

void scaleCurve(vector<glm::vec3*>* points, float factor) {
	vector<glm::vec3*>* scaledCurvePoints = new vector<glm::vec3*>();

	for (int i = 0; i < points->size(); i++) {
		scaledCurvePoints->push_back(new glm::vec3(points->at(i)->x * factor, points->at(i)->y, points->at(i)->z * factor));
	}
	curvePoints = scaledCurvePoints;
}

void preparePrintingData(vector<Object*> objects) {
	vector<float> vs;

	for (int o = 0; o < objects.size(); o++) {
		Mesh* mesh = objects[o]->getMesh();

		mesh->getShader()->setInt("material.tex", 0);

		for (int i = 0; i < mesh->getGroups().size(); i++) {
			vector<Face*> faces = mesh->getGroups()[i]->getFaces();
			for (int j = 0; j < faces.size(); j++) {
				vector<int> vertsIndex = faces[j]->getVertices();
				vector<int> textsIndex = faces[j]->getTextures();
				vector<int> normsIndex = faces[j]->getNormals();

				for (int k = 0; k < vertsIndex.size(); k++) {
					glm::vec3 n;
					glm::vec2 t;
					glm::vec3 v = mesh->getVertex()[vertsIndex[k]];
					if (textsIndex.size() != 0) t = mesh->getTexture()[textsIndex[k]];
					if (normsIndex.size() != 0) n = mesh->getNormals()[normsIndex[k]];

					vs.push_back(v.x);
					vs.push_back(v.y);
					vs.push_back(v.z);
					vs.push_back(t.x);
					vs.push_back(t.y);
					vs.push_back(n.x);
					vs.push_back(n.y);
					vs.push_back(n.z);
				}
			}

			GLuint vbo = 0, vao = 0;

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(float), vs.data(), GL_STATIC_DRAW);

			//vertex
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

			//texture
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

			//normal
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

			mesh->getGroups()[i]->setVAOIndex(vao);

			if (mesh->getGroups()[i]->getMaterial() != "") {

				Material* material = getMaterialObject(mesh->getGroups()[i]->getMaterial());

				if (material != nullptr && material->getTextureFile() != "") {
					unsigned int textureID;

					glGenTextures(1, &textureID);
					glBindTexture(GL_TEXTURE_2D, textureID);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					int width, height, nrChannels;

					unsigned char* data = stbi_load(material->getTextureFile().c_str(), &width, &height, &nrChannels, 0);
					if (data)
					{

						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
						glGenerateMipmap(GL_TEXTURE_2D);
					}
					else
					{
						std::cout << "Texture failed to load at path: " << material->getTextureFile().c_str() << std::endl;
					}

					mesh->getGroups()[i]->setTextureIndex(textureID);
					stbi_image_free(data);
					glBindTexture(GL_TEXTURE_2D, 0);

					mesh->getShader()->use();

					//ligthing
					//Ka
					glm::vec3 Ka = material->getKa();
					if (Ka.r == 0 && Ka.g == 0 && Ka.b == 0)
						mesh->getShader()->setVec3("Ka", 1.0, 1.0, 1.0);
					else
						mesh->getShader()->setVec3("Ka", Ka.r, Ka.g, Ka.b);

					//Kd
					glm::vec3 Kd = material->getKd();
					if (Kd.r == 0 && Kd.g == 0 && Kd.b == 0)
						mesh->getShader()->setVec3("Kd", 1.0, 0.5, 0.0);
					else
						mesh->getShader()->setVec3("Kd", Kd.r, Kd.g, Kd.b);

					//Ks
					glm::vec3 Ks = material->getKs();
					if (Ks.r == 0 && Ks.g == 0 && Ks.b == 0)
						mesh->getShader()->setVec3("Ks", 1.0, 1.0, 1.0);
					else
						mesh->getShader()->setVec3("Ks", Ks.r, Ks.g, Ks.b);

					//Ns
					float Ns = material->getNs();
					if (Ns == 0)
						mesh->getShader()->setFloat("Ns", 100);
					else
						mesh->getShader()->setFloat("Ns", Ns);
				}
			}
			glBindVertexArray(0);
			vs.clear();
		}
	}
}

float calculateDirection(int inda, int indb) {
	glm::vec3* a = curvePoints->at(inda);
	glm::vec3* b;

	if (inda == curvePoints->size() - 5) {
		b = curvePoints->at(0);
	}
	else {
		b = curvePoints->at(indb);
	}

	GLfloat dx = b->x - a->x;
	GLfloat dz = b->z - a->z;

	GLfloat angle = glm::atan(dz, dx);

	return angle;
}

Material* getMaterialObject(string materialId) {
	for (int i = 0; i < materials.size(); i++) {
		if (materials[i]->getMaterialid() == materialId) {
			return materials[i];
		}
	}
	return nullptr;
}

void draw(vector<Object*> objects) {
	int movementIndex = 0;
	float angle = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(objects);

		glClearColor(0.8f, 0.8f, 0.8f, 0.8f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		for (int o = 0; o < objects.size(); o++) {
			Mesh* mesh = objects[o]->getMesh();

			mesh->getShader()->use();
			mesh->getShader()->setMat4("view", view);
			mesh->getShader()->setMat4("projection", projection);

			glm::mat4 model(1);

			for (int i = 0; i < mesh->getGroups().size(); i++) {
				GLuint VAO = mesh->getGroups()[i]->getVAOIndex();
				glBindVertexArray(VAO);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mesh->getGroups()[i]->getTextureIndex());

				if (mesh->getGroups()[i]->getMaterial() != "") {
					Material* material = getMaterialObject(mesh->getGroups()[i]->getMaterial());
				}
				
				if (mesh->getGroups()[i]->getName() == "road") {
					model = glm::scale(model, glm::vec3(2.0f));
				}
				else {
					model = glm::scale(model, glm::vec3(0.25f));
					model = glm::translate(model, glm::vec3(curvePoints->at(movementIndex)->x, curvePoints->at(movementIndex)->y + 0.5f, curvePoints->at(movementIndex)->z));
					angle = -calculateDirection(movementIndex, movementIndex + 5);
					angle += 6.2;
					model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
				}

				objects[o]->setModel(model);
				mesh->getShader()->setMat4("model", objects[o]->getModel());

				glDrawArrays(GL_TRIANGLES, 0, mesh->getGroups()[i]->getFaces().size() * 3);
			}
		}
		movementIndex += 1;
		if (curvePoints->size() - 5 == movementIndex)
			movementIndex = 0;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}