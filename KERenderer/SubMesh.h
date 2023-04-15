#pragma once
#include "kmath.h"
#include "macro.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "api.h"

class SubMesh {
public:
	TGAimage* diffuse;
	TGAimage* normal_map;
	kmath::vec3f Ka, Ks, Kd;
	std::vector<std::vector<kmath::vec3i>> face;
	SubMesh() : 
		diffuse(NULL),
		normal_map(NULL),
		Ka(1.f, 1.f, 1.f),
		Ks(1.f, 1.f, 1.f),
		Kd(1.f, 1.f, 1.f),
		face() { }
};