#pragma once
#include "Mesh.h"
#include "draw2d.h"

class Shader
{
   Mesh* mesh;
public:
	virtual void vert() = 0;
	virtual void frag() = 0;
};

