#include "MtlWriter.h"

MtlWriter::MtlWriter(){}

void MtlWriter::open() {
	ofstream file;
	file.open("curve.mtl");
	file << "newmtl " << "road\n" << endl;
	file << "Kd " << 1.0 << " " << 0.5 << " " << 0.0 << endl;
	file << "Ka " << 1.0 << " " << 1.0 << " " << 1.0 << endl;
	file << "Tf " << 1.0 << " " << 1.0 << " " << 1.0 << endl;
	file << "map_Kd " << "road.png" << endl;
	file << "Ni " << 1.0 << endl;
	file << "Ns " << 100.0 << endl;
	file << "Ks " << 1.0 << " " << 1.0 << " " << 1.0 << endl;
	file.close();
}