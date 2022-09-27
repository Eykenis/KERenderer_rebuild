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
	std::vector<kmath::vec3f> tex_coord;
	std::vector<std::vector<kmath::vec3i>> face;

	Mesh(const char* filename) : vert(), face() {
		std::ifstream in;
		in.open(filename, std::ifstream::in);
		if (in.fail()) return;

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
		}
	}

	void vertNormalize() {
		float xl = 1e10, xr = -1e10, yu = -1e10, yd = 1e10;
		for (int i = 0; i < vert.size(); ++i) {
			xl = min(xl, vert[i].x);
			xr = max(xr, vert[i].x);
			yd = min(yd, vert[i].y);
			yu = max(yu, vert[i].y);
		}
		float prop = (xr - xl) / (yu - yd);
		for (int i = 0; i < vert.size(); ++i) {
			vert[i].x = ((vert[i].x - xl) / (xr - xl) * WINDOW_HEIGHT) * prop;
			vert[i].y = ((vert[i].y - yd) / (yu - yd) * WINDOW_HEIGHT);
		}
	}
};

