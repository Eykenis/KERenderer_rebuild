#pragma once
#include "SubMesh.h"

/*
* Support Material Library File
* PLEASE put .mtl files along with .obj files in path "./obj/..." !
*/

class Mesh
{
public:
	std::string mtlname;
	std::vector<kmath::vec3f> vert;
	std::vector<kmath::vec3f> normal;
	std::vector<kmath::vec2f> tex_coord;
	std::vector<SubMesh> submesh;
	std::map<std::string, int> mesh_place;

	Mesh(const char* filename, const char* diffuse_name = NULL, const char* normal_name = NULL) {
		std::ifstream in;
		in.open(filename, std::ifstream::in);
		if (in.fail()) return;

		std::string line;

		// check if there is mtllib
		while (!in.eof()) {
			std::getline(in, line);
			std::istringstream iss(line.c_str());
			std::string trash;
			if (!line.compare(0, 7, "mtllib ")) {
				iss >> trash >> mtlname;
				break;
			}
			if (!line.compare(0, 2, "# ")) {
				continue;
			}
			else break;
		}

		// rewind in filepointer to start of file
		in.clear();
		in.seekg(0, ios::beg);

		// read mtl file and create submeshes.
		if (!mtlname.empty()) {
			int mesh_index = -1;
			std::ifstream mtl;
			mtl.open("./obj/" + mtlname, std::ifstream::in);
			if (mtl.fail()) {
				cout << "Open " << mtlname << " failed." << endl;
				return;
			}
			std::string trash;
			while (!mtl.eof()) {
				std::getline(mtl, line);
				std::istringstream iss(line.c_str());
				if (!line.compare(0, 7, "newmtl ")) {
					iss >> trash;
					mesh_index++;
					submesh.push_back(SubMesh());
					iss >> trash;
					mesh_place[trash] = mesh_index;
				}
				else if (!line.compare(0, 3, "Ka ")) {
					iss >> trash;
					iss >> submesh[mesh_index].Ka.v[0] >> submesh[mesh_index].Ka.v[1] >> submesh[mesh_index].Ka.v[2];
				}
				else if (!line.compare(0, 3, "Ks ")) {
					iss >> trash;
					iss >> submesh[mesh_index].Ks.v[0] >> submesh[mesh_index].Ks.v[1] >> submesh[mesh_index].Ks.v[2];
				}
				else if (!line.compare(0, 3, "Kd ")) {
					iss >> trash;
					iss >> submesh[mesh_index].Kd.v[0] >> submesh[mesh_index].Kd.v[1] >> submesh[mesh_index].Kd.v[2];
				}
				else if (!line.compare(0, 2, "d ")) {
					iss >> trash;
					iss >> submesh[mesh_index].d;
				}
				else if (!line.compare(0, 7, "map_Kd ")) {
					iss >> trash;
					submesh[mesh_index].diffuse = new TGAimage;
					iss >> trash;
					submesh[mesh_index].diffuse->read_TGA(("./tex/" + trash).c_str());
				}
			}
		}
		else {
			// no MTL file
			submesh.push_back(SubMesh());
			if (diffuse_name) {
				submesh[0].diffuse = new TGAimage;
				submesh[0].diffuse->read_TGA(diffuse_name);
			}
			if (normal_name) {
				submesh[0].normal_map = new TGAimage;
				submesh[0].normal_map->read_TGA(normal_name);
			}
		}
		
		int mesh_index = 0;
		while (!in.eof()) {
			std::getline(in, line);
			std::istringstream iss(line.c_str());
			std::string trash;
			char ctrash;

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
				while (iss >> tmp.v[0] >> ctrash >> tmp.v[1] >> ctrash >> tmp.v[2]) {
					for (int i = 0; i < 3; ++i) {
						tmp.v[i]--;
					}
					f.push_back(tmp);
				}
				submesh[mesh_index].face.push_back(f);
			}
			else if (!line.compare(0, 3, "vt ")) {
				iss >> trash;
				kmath::vec2f v;
				for (int i = 0; i < 2; ++i) {
					iss >> v.v[i];
				}
				tex_coord.push_back(v);
			}
			else if (!line.compare(0, 3, "vn ")) {
				iss >> trash;
				kmath::vec3f v;
				for (int i = 0; i < 3; ++i) {
					iss >> v.v[i];
				}
				normal.push_back(v);
			}
			else if (!line.compare(0, 7, "usemtl ")) {
				iss >> trash;
				iss >> trash;
				mesh_index = mesh_place[trash];
			}
		}
	}
};

