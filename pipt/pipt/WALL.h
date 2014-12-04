#ifndef WALL_H
#define WALL_H

class WALL_VERTEX
{
public:
	VECTOR3D position;
	float s, t;
	VECTOR3D sTangent, tTangent;
	VECTOR3D normal;
	VECTOR3D tangentSpaceLight;
};

class WALL
{
public:
	WALL();
	~WALL();

	bool InitWALL();

	int numVertices;
	int numIndices;


	unsigned int * indices;
	WALL_VERTEX * vertices;
};

const int wallPrecision=4;
const float wallHeight=4.0f;
const float wallWidth=4.0f;

#endif	//WALL_H