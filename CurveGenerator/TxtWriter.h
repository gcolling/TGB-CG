#ifndef TXTWRITER_H
#define TXTWRITER_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

class TxtWriter {
public:
	ofstream file;

	TxtWriter();
	void open();
	void addPoint(float x, float y, float z);
	void close();
};

#endif