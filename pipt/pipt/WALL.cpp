#include <windows.h>
#include <stdio.h>
#include <GL\gl.h>
#include "Maths/Maths.h"
#include "WALL.h"

WALL::WALL()
{
	InitWALL();
}

WALL::~WALL()
{
	if(indices)
		delete [] indices;
	indices=NULL;

	if(vertices)
		delete [] vertices;
	vertices=NULL;
}

bool WALL::InitWALL()
{
	numVertices=(wallPrecision+1)*(wallPrecision+1);
	numIndices=(wallPrecision+1)*(wallPrecision+1)*2*3;

	numVertices=(wallPrecision+1)*(wallPrecision+1);
	numIndices=2*wallPrecision*wallPrecision*3;

	vertices=new WALL_VERTEX[numVertices];
	if(!vertices)
	{
		printf("Unable to allocate memory for torus vertices\n");
		return false;
	}

	indices=new unsigned int[numIndices];
	if(!indices)
	{
		printf("Unable to allocate memory for torus indices\n");
		return false;
	}

	int index;
	VECTOR3D normal=VECTOR3D(0,1,0).CrossProduct(VECTOR3D(1,0,0));
	for(int j=0;j<=wallPrecision;j++){
		for(int i=0;i<=wallPrecision;i++){
			index=j*(wallPrecision+1)+i;
			vertices[index].position=VECTOR3D(0,0,0)+VECTOR3D((float)i*wallWidth/wallPrecision,(float)j*wallHeight/wallPrecision,0);
			vertices[index].t=(float)i/wallPrecision;
			vertices[index].s=(float)j/wallPrecision;

			vertices[index].sTangent.Set(0.0f, 1.0f, 0.0f);
			vertices[index].tTangent.Set(1.0f, 0.0f, 0.0f);
			vertices[index].normal=vertices[index].tTangent.CrossProduct(vertices[index].sTangent);
		}
	}



	//calculate the indices
	for(int j=0; j<wallPrecision; j++)
	{
		for(int i=0; i<wallPrecision; i++)
		{
			indices[((i+j*wallPrecision)*2)*3+0]=i+j*(wallPrecision+1);//0
			indices[((i+j*wallPrecision)*2)*3+1]=i+(j+1)*(wallPrecision+1);//3;
			indices[((i+j*wallPrecision)*2)*3+2]=i+1+j*(wallPrecision+1);//1
			indices[((i+j*wallPrecision)*2+1)*3+0]=i+1+j*(wallPrecision+1);//1
			indices[((i+j*wallPrecision)*2+1)*3+1]=i+(j+1)*(wallPrecision+1);//3
			indices[((i+j*wallPrecision)*2+1)*3+2]=i+1+(j+1)*(wallPrecision+1);//4
		}
	}
	return true;
}