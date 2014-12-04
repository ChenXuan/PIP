#ifndef FLOOR_H
#define FLOOR_H

class FLOOR_VERTEX
{
public:
	VECTOR3D position;
	float s, t;
	VECTOR3D normal;
};

class FLOOR
{
public:
	FLOOR();
	~FLOOR();


	bool InitFLOOR();

	int numVertices;
	int numIndices;


	unsigned int * indices;
	FLOOR_VERTEX * vertices;
};

const int floorPrecision=2;
const float floorHeight=15.0f;
const float floorWidth=15.0f;

#endif	