#ifndef CEIL_H
#define CEIL_H

class CEIL_VERTEX
{
public:
	VECTOR3D position;
	float s, t;
	VECTOR3D normal;
};

class CEIL
{
public:
	CEIL();
	~CEIL();


	bool InitCEIL();

	int numVertices;
	int numIndices;


	unsigned int * indices;
	CEIL_VERTEX * vertices;
};

const int ceilPrecision=500;
const float ceilHeight=15.0f;
const float ceilWidth=15.0f;

#endif	//CEIL_H