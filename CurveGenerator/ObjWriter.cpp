#include "ObjWriter.h"

ObjWriter::ObjWriter() {}

void ObjWriter::create() {
	file.open("curve.obj");
	file << "mtllib " << "curve.mtl" << "\n" << endl;
	file << "g " << "road" << "\n" << endl;
	file << "usemtl road\n" << endl;
	file.close();
}

void ObjWriter::addFaces(int index, int curveSize, int faces, int curveType) {
	file.open("curve.obj", ios::app);
	if (curveType == 1) {
		file << "f " << index + curveSize << "/" << 1 << "/" << faces << " " <<
			index + 1 + curveSize << "/" << 2 << "/" << faces << " " <<
			index << "/" << 3 << "/" << faces << endl;
	}
	if (curveType == 2) {
		file << "f " << index + 1 + curveSize << "/" << 2 << "/" << faces << " " <<
			index + 1 << "/" << 4 << "/" << faces << " " <<
			index << "/" << 3 << "/" << faces << endl;
	}
	if (curveType == 3) {
		file << "f " << index + curveSize << "/" << 1 << "/" << faces << " " <<
			curveSize + 1 << "/" << 2 << "/" << faces << " " <<
			index << "/" << 3 << "/" << faces << endl;
	}
	if (curveType == 4) {
		file << "f " << 1 + curveSize << "/" << 2 << "/" << faces << " " <<
			1 << "/" << 4 << "/" << faces << " " <<
			index << "/" << 3 << "/" << faces << endl;
	}
	file.close();
}

void ObjWriter::addNormals(glm::vec3 normalI, glm::vec3 normalE) {
	file.open("curve.obj", ios::app);
	file << "vn " << normalI[0] << " " << normalI[1] << " " << normalI[2] << endl;
	file << "vn " << normalE[0] << " " << normalE[1] << " " << normalE[2] << endl;
	file.close();
}

void ObjWriter::addPoints(float x, float y, float z) {
	file.open("curve.obj", ios::app);
	file << "v " << x << " " << z << " " << y << endl;
	file.close();
}

void ObjWriter::saveTexture() {
	file.open("curve.obj", ios::app);
	file << endl;
	file << "vt " << 1.0 << " " << 1.0 << endl;
	file << "vt " << 1.0 << " " << 0.0 << endl;
	file << "vt " << 0.0 << " " << 1.0 << endl;
	file << "vt " << 0.0 << " " << 0.0 << endl;
	file << endl;
	file.close();
}