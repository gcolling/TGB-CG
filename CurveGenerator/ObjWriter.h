#ifndef OBJWRITER_H
#define OBJWRITER_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

using namespace std;

class ObjWriter {
public:
	ofstream file;

	ObjWriter();
	void create();
	void addFaces(int index, int curveSize, int faces, int curveType);
	void addNormals(glm::vec3 normalI, glm::vec3 normalE);
	void addPoints(float x, float y, float z);
	void saveTexture();
};

#endif