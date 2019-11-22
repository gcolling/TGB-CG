#include "Group.h"

#include <iostream>
using namespace std;

Group::Group(string name, string material) {
	this->name = name;
	this->material = material;
}

void Group::setName(string name) {
	this->name = name;
}

string Group::getName() {
	return this->name;
}

void Group::addFace(Face* face) {
	this->faces.push_back(face);
}

void Group::addFace(vector<int> verts, vector<int> texts, vector<int> norms) {
	Face* newFace = new Face();
	newFace->setInfo(verts, texts, norms);

	this->faces.push_back(newFace);
}

vector<Face*> Group::getFaces() {
	return this->faces;
}

void Group::setMaterial(string material) {
	this->material = material;
}

string Group::getMaterial() {
	return this->material;
}

void Group::setTextureIndex(GLuint index) {
	this->textureIndex = index;
}

GLuint Group::getTextureIndex() {
	return this->textureIndex;
}

void Group::setVAOIndex(GLuint vaoIndex) {
	this->VAO = vaoIndex;
}

GLuint Group::getVAOIndex() {
	return this->VAO;
}