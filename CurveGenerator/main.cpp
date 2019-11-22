#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "MTLWriter.h"
#include "TXTWriter.h"
#include "OBJWriter.h"

using namespace std;

const float ALTERNANCE = 3.14159265359;
const float HALF_ALTERNANCE = ALTERNANCE / 2.0;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


vector<glm::vec3*>* selection = new vector<glm::vec3*>();
vector<glm::vec3*>* rawCurve = new vector<glm::vec3*>();
vector<glm::vec3*>* externalCurve = new vector<glm::vec3*>();
vector<glm::vec3*>* internalCurve = new vector<glm::vec3*>();
vector<glm::vec3*>* finalCurve = new vector<glm::vec3*>();
vector<GLfloat>* finalFloat = new vector<GLfloat>();

int internalLength = 0, externalLength = 0, faces = 0;
bool shouldDraw = false;

Shader* mainShader;

GLFWwindow* window;
GLuint vao, vbo;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void normalizeAxes(double& x, double& y);

int getQuadrant(float x, float y);

float calculateLineX(vector<glm::vec3*>* refil, float t, int index);
float calculateLineY(vector<glm::vec3*>* refil, float t, int index);

vector<glm::vec3*>* getLine(vector<glm::vec3*>* points);
vector<glm::vec3*>* getCurve(vector<glm::vec3*>* points, bool external);
vector<glm::vec3*>* getFinal(vector<glm::vec3*>* internal, vector<glm::vec3*>* external);

vector<GLfloat>* getFloat(std::vector<glm::vec3*>* points);

int main() {
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
	glewExperimental = GL_TRUE;
	glewInit();

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	mainShader = new Shader("vs.glsl", "fs.glsl");
	mainShader->use();

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	MtlWriter MtlW;
	MtlW.open();
	ObjWriter ObjW;
	ObjW.create();

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		if (shouldDraw == true) {
			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, finalFloat->size());
		}
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}

vector<GLfloat>* getFloat(std::vector<glm::vec3*>* points) {

	vector<GLfloat>* converted = new std::vector<GLfloat>();

	for (int i = 0; i < points->size(); i++) {
		converted->push_back(points->at(i)->x);
		converted->push_back(points->at(i)->y);
		converted->push_back(points->at(i)->z);
	}
	return converted;
}

void normalizeAxes(double& x, double& y) {
	if (x > (SCR_WIDTH / 2)) {
		x = x - (SCR_WIDTH / 2);
		x = x / (SCR_WIDTH / 2);
	}
	else if (x == (SCR_WIDTH / 2))
		x = 0;
	else
		x = -(((SCR_WIDTH / 2) - x) / (SCR_WIDTH / 2));

	if (y > (SCR_HEIGHT / 2)) {
		y = y - (SCR_HEIGHT / 2);
		y = y / (SCR_HEIGHT / 2);
		y = y * (-1);
	}
	else if (y == (SCR_HEIGHT / 2))
		y = 0;
	else {
		y = -(((SCR_HEIGHT / 2) - y) / (SCR_HEIGHT / 2));
		y = y * (-1);
	}
}

int getQuadrant(float x, float y) {
	if (x > 0.0 && y > 0.0)
		return 1;
	else if (x < 0.0 && y > 0.0)
		return 2;
	else if (x < 0.0 && y < 0.0)
		return 3;
	else
		return 4;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		normalizeAxes(xpos, ypos);

		glm::vec3* point = new glm::vec3(xpos, ypos, 0.0);
		selection->push_back(point);

		int quadrant = getQuadrant(xpos, ypos);
		if (quadrant == 1) {
			xpos += 0.5;
			ypos += 0.5;
		}
		else if (quadrant == 2) {
			xpos -= 0.5;
			ypos += 0.5;
		}
		else if (quadrant == 3) {
			xpos -= 0.5;
			ypos -= 0.5;
		}
		else if (quadrant == 4) {
			xpos += 0.5;
			ypos -= 0.5;
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {	
		rawCurve = getLine(selection);
		externalCurve = getCurve(rawCurve, true);
		internalCurve = getCurve(rawCurve, false);

		externalLength = externalCurve->size() / 2.0;
		internalLength = internalCurve->size() / 2.0;

		finalCurve = getFinal(internalCurve, externalCurve);
		finalFloat = getFloat(finalCurve);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * finalFloat->size(), &finalFloat->at(0), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		
		ObjWriter objW;
		objW.saveTexture();

		shouldDraw = true;
	}
}

vector<glm::vec3*>* getLine(vector<glm::vec3*>* points) {
	vector<glm::vec3*>* curve = new vector<glm::vec3*>();
	vector<glm::vec3*>* refil = new vector<glm::vec3*>();

	glm::vec3* point;
	
	TxtWriter txtW;
	txtW.open();
	
	for (int i = 0; i < points->size(); i++) {
		refil->push_back(new glm::vec3(points->at(i)->x, points->at(i)->y, 0));
	}

	refil->push_back(points->at(0));
	refil->push_back(points->at(1));
	refil->push_back(points->at(2));

	for (int i = 0; i < (refil->size() - 3); i++) {
		for (int j = 0; j < 500; ++j) {
			float t = static_cast<float>(j) / 499.0;

			GLfloat x = calculateLineX(refil, t, i);
			GLfloat y = calculateLineY(refil, t, i);

			point = new glm::vec3(x, y, 0.0);
			curve->push_back(point);
			txtW.addPoint(point->x, point->y, point->z);
			curve->push_back(new glm::vec3(1.0, 0.0, 1.0));
		}
	}
	txtW.close();

	return curve;
}

float calculateLineX(vector<glm::vec3*>* refil, float t, int index){
	return (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * refil->at(index)->x +
			(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4) * refil->at(index + 1)->x +
			(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) * refil->at(index + 2)->x +
			(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0) * refil->at(index + 3)->x) / 6);
}

float calculateLineY(vector<glm::vec3*>* refil, float t, int index) {
	return (((-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * refil->at(index)->y +
		(3 * pow(t, 3) - 6 * pow(t, 2) + 0 * t + 4) * refil->at(index + 1)->y +
		(-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) * refil->at(index + 2)->y +
		(1 * pow(t, 3) + 0 * pow(t, 2) + 0 * t + 0) * refil->at(index + 3)->y) / 6);
}

vector<glm::vec3*>* getCurve(vector<glm::vec3*>* points, bool external) {
	vector<glm::vec3*>* curve = new vector<glm::vec3*>();
	ObjWriter ObjW;

	for (int j = 0; j < points->size() - 1; j += 2) {
		GLfloat angle, distanceX, distanceY, offsetX, offsetY;
		glm::vec3* fPoint = points->at(j);
		glm::vec3* lPoint;

		if (j == points->size() - 2)
			lPoint = points->at(0);
		else
			lPoint = points->at(j + 2);

		distanceX = lPoint->x - fPoint->x;
		distanceY = lPoint->y - fPoint->y;

		if (distanceX == 0 || distanceY == 0) {
			distanceX = lPoint->x - points->at(j - 2)->x;
			distanceY = lPoint->y - points->at(j - 2)->y;
		}

		angle = glm::atan(distanceY, distanceX);

		if (external)
			angle += HALF_ALTERNANCE;
		else
			angle -= HALF_ALTERNANCE;

		offsetX = glm::cos(angle) * 0.09;
		offsetY = glm::sin(angle) * 0.09;

		glm::vec3* curvePoints = new glm::vec3(fPoint->x + offsetX, fPoint->y + offsetY, 0.0);
		curve->push_back(curvePoints);
		ObjW.addPoints(curvePoints->x, curvePoints->y, curvePoints->z);
		curve->push_back(new glm::vec3(1.0, 0.0, 1.0));

	}
	return curve;
}

vector<glm::vec3*>* getFinal(vector<glm::vec3*>* internal, vector<glm::vec3*>* external) {
	ObjWriter ObjW;

	int i = 0;
	int index = 1;

	for (; i < internal->size() - 2; i += 2) {
		finalCurve->push_back(internal->at(i));
		finalCurve->push_back(internal->at(i + 1));
		glm::vec3* fPointInt = internal->at(i);

		finalCurve->push_back(internal->at(i + 2));
		finalCurve->push_back(internal->at(i + 3));
		glm::vec3* lPointInt = internal->at(i + 2);

		finalCurve->push_back(external->at(i));
		finalCurve->push_back(external->at(i + 1));
		glm::vec3* fPointExt = external->at(i);
		
		ObjW.addFaces(index, externalLength, ++faces, 1);

		finalCurve->push_back(internal->at(i + 2));
		finalCurve->push_back(internal->at(i + 3));

		finalCurve->push_back(external->at(i + 2));
		finalCurve->push_back(external->at(i + 3));
		glm::vec3* lPointExt = external->at(i + 2);

		finalCurve->push_back(external->at(i));
		finalCurve->push_back(external->at(i + 1));

		ObjW.addFaces(index, externalLength, ++faces, 2);

		glm::vec3 ab = glm::vec3(lPointInt->x - fPointInt->x, lPointInt->z - fPointInt->z, lPointInt->y - fPointInt->y);
		glm::vec3 ac = glm::vec3(fPointExt->x - fPointInt->x, fPointExt->z - fPointInt->z, fPointExt->y - fPointInt->y);
		glm::vec3 dc = glm::vec3(fPointExt->x - lPointExt->x, fPointExt->z - lPointExt->z, fPointExt->y - lPointExt->y);
		glm::vec3 db = glm::vec3(lPointInt->x - lPointExt->x, lPointInt->z - lPointExt->z, lPointInt->y - lPointExt->y);
		glm::vec3 internalNormal = glm::cross(ac, ab);
		glm::vec3 externalNormal = glm::cross(db, dc);

		ObjW.addNormals(internalNormal, externalNormal);
		index++;
	}

	finalCurve->push_back(internal->at(i));
	finalCurve->push_back(internal->at(i + 1));
	glm::vec3* fPointInt = internal->at(i);

	finalCurve->push_back(internal->at(0));
	finalCurve->push_back(internal->at(1));
	glm::vec3* lPointInt = internal->at(0);

	finalCurve->push_back(external->at(i));
	finalCurve->push_back(external->at(i + 1));
	glm::vec3* fPointExt = external->at(i);

	ObjW.addFaces(index, externalLength, ++faces, 3);

	finalCurve->push_back(internal->at(0));
	finalCurve->push_back(internal->at(1));

	finalCurve->push_back(external->at(0));
	finalCurve->push_back(external->at(1));
	glm::vec3* lPointExt = external->at(0);

	finalCurve->push_back(external->at(i));
	finalCurve->push_back(external->at(i + 1));

	ObjW.addFaces(index, externalLength, ++faces, 4);

	glm::vec3 ab = glm::vec3(fPointInt->x - lPointInt->x, fPointInt->z - lPointInt->z, fPointInt->y - lPointInt->y);
	glm::vec3 ac = glm::vec3(fPointInt->x - fPointExt->x, fPointInt->z - fPointExt->z, fPointInt->y - fPointExt->y);
	glm::vec3 dc = glm::vec3(lPointExt->x - fPointExt->x, lPointExt->z - fPointExt->z, lPointExt->y - fPointExt->y);
	glm::vec3 db = glm::vec3(lPointExt->x - lPointInt->x, lPointExt->z - lPointInt->z, lPointExt->y - lPointInt->y);
	glm::vec3 internalNormal = glm::cross(ab, ac);
	glm::vec3 externalNormal = glm::cross(db, dc);

	ObjW.addNormals(internalNormal, externalNormal);

	return finalCurve;
}
