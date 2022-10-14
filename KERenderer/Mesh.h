#pragma once
#include "kmath.h"
#include "macro.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "api.h"

class Mesh
{
public:
	std::vector<kmath::vec3f> vert;
	std::vector<kmath::vec3f> normal;
	std::vector<kmath::vec2f> tex_coord;
	std::vector<std::vector<kmath::vec3i>> face;
	TGAimage* diffuse;
	char mtl_filename[128];

	Mesh(const char* filename, const char* diffuse_name) : vert(), face() {
		std::ifstream in;
		in.open(filename, std::ifstream::in);
		if (in.fail()) return;

		diffuse = new TGAimage;

		std::string line;
		while (!in.eof()) {
			std::getline(in, line);
			std::istringstream iss(line.c_str());
			char trash;

			if (!line.compare(0, 2, "v ")) {
				iss >> trash;
				kmath::vec3f v;
				for (int i = 0; i < 3; ++i) {
					iss >> v.v[i];
				}
				vert.push_back(v);
			}
			else if (!line.compare(0, 2, "f ")) {
				iss >> trash;
				kmath::vec3i tmp;
				std::vector<kmath::vec3i> f;
				while (iss >> tmp.v[0] >> trash >> tmp.v[1] >> trash >> tmp.v[2]) {
					for (int i = 0; i < 3; ++i) {
						tmp.v[i]--;
					}
					f.push_back(tmp);
				}
				face.push_back(f);
			}
			else if (!line.compare(0, 3, "vt ")) {
				iss >> trash >> trash;
				kmath::vec2f v;
				for (int i = 0; i < 2; ++i) {
					iss >> v.v[i];
				}
				tex_coord.push_back(v);
			}
			else if (!line.compare(0, 3, "vn ")) {
				iss >> trash >> trash;
				kmath::vec3f v;
				for (int i = 0; i < 3; ++i) {
					iss >> v.v[i];
				}
				normal.push_back(v);
			}
		}
		if (!diffuse_name) {
			delete diffuse;
			diffuse = NULL;
		}
		else diffuse->read_TGA(diffuse_name);
	}
};

