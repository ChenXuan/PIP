#include <windows.h>
#include <stdio.h>
#include <GL\gl.h>
#include "Asist/Maths.h"
#include "FLOOR.h"

FLOOR::FLOOR()
{
	InitFLOOR();
}

FLOOR::~FLOOR()
{
	if(indices)
		delete [] indices;
	indices=NULL;

	if(vertices)
		delete [] vertices;
	vertices=NULL;
}

bool FLOOR::InitFLOOR(){
	numVertices=(floorPrecision+1)*(floorPrecision+1);
	numIndices=4*floorPrecision*floorPrecision;

	vertices=new FLOOR_VERTEX[numVertices];
	if(!vertices)
	{
		printf("Unable to allocate memory for floor vertices\n");
		return false;
	}

	indices=new unsigned int[numIndices];
	if(!indices)
	{
		printf("Unable to allocate memory for floor indices\n");
		return false;
	}

	int index=0;
	for(int i=0;i<=floorPrecision;i++){
		for(int j=0;j<=floorPrecision;j++)
		{
			index=j*(floorPrecision+1)+i;
			vertices[index].position=VECTOR3D(0,0,0)+VECTOR3D((float)i*floorWidth/floorPrecision,0,-(float)j*floorHeight/floorPrecision);
			vertices[index].t=(float)i/floorPrecision;
			vertices[index].s=(float)j/floorPrecision;
			vertices[index].normal.Set(0,1,0);
		}
	}
	
	//calculate the indices
	for(int j=0; j<floorPrecision; j++)
	{
		for(int i=0; i<floorPrecision; i++)
		{
			indices[(floorPrecision*j+i)*4+0]=i+(floorPrecision+1)*j;
			indices[(floorPrecision*j+i)*4+1]=i+1+(floorPrecision+1)*j;
			indices[(floorPrecision*j+i)*4+2]=i+1+(floorPrecision+1)*(j+1);
			indices[(floorPrecision*j+i)*4+3]=i+(floorPrecision+1)*(j+1);
		}
	}
	
}