#include "TxtWriter.h"

TxtWriter::TxtWriter() {}

void TxtWriter::open() {
	file.open("originalCurve.txt");
}

void TxtWriter::addPoint(float x, float y, float z) {
	file << "v " << x << " " << z << " " << y << endl;
}
void TxtWriter::close() {
	file.close();
}