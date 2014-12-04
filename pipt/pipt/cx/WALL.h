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

const int wallPrecision=3;
const float wallHeight=15.0f;
const float wallWidth=15.0f;

#endif	//WALL_H