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
	numVertices=(wallPrecision+1)*(wallPrecision+1)*2;
	numIndices=4*wallPrecision*wallPrecision*2;

	vertices=new WALL_VERTEX[numVertices];
	if(!vertices)
	{
		printf("Unable to allocate memory for wall vertices\n");
		return false;
	}

	indices=new unsigned int[numIndices];
	if(!indices)
	{
		printf("Unable to allocate memory for wall indices\n");
		return false;
	}
	
	//wall1
	int index;
	VECTOR3D normal=VECTOR3D(1.0f, 0.0f, 0.0f).CrossProduct(VECTOR3D(0.0f, 1.0f, 0.0f));
	for(int j=0;j<=wallPrecision;j++){
		for(int i=0;i<=wallPrecision;i++){
			index=j*(wallPrecision+1)+i;
			vertices[index].position=VECTOR3D(0,0,0)+VECTOR3D((float)i*wallWidth/wallPrecision,(float)j*wallHeight/wallPrecision,0);
			
			vertices[index].t=(float)i/wallPrecision;
			vertices[index].s=(float)j/wallPrecision;

			vertices[index].sTangent.Set(0.0f, 1.0f, 0.0f);
			vertices[index].tTangent.Set(1.0f, 0.0f, 0.0f);
			vertices[index].normal.Set(normal.GetX(),normal.GetY(),normal.GetZ());//=vertices[index].tTangent.CrossProduct(vertices[index].sTangent);
		}
	}
	//wall2
	int index2;
    normal=VECTOR3D(0.0f, 0.0f, -1.0f).CrossProduct(VECTOR3D(0.0f, 1.0f, 0.0f));
	for(int j=0;j<=wallPrecision;j++){
		for(int i=0;i<=wallPrecision;i++){
			index2=j*(wallPrecision+1)+i+index+1;
			vertices[index2].position=VECTOR3D(0,0,0)+VECTOR3D(0,(float)j*wallHeight/wallPrecision,-(float)i*wallWidth/wallPrecision);
			vertices[index2].t=(float)i/wallPrecision;
			vertices[index2].s=(float)j/wallPrecision;

			vertices[index2].sTangent.Set(0.0f, 1.0f, 0.0f);
			vertices[index2].tTangent.Set(0.0f, 0.0f, -1.0f);
			vertices[index2].normal.Set(normal.GetX(),normal.GetY(),normal.GetZ());//=vertices[index].tTangent.CrossProduct(vertices[index].sTangent);
		}
	}
	
	int indiceswall1=4*wallPrecision*wallPrecision;
	//calculate the indices
	for(int j=0; j<wallPrecision; j++)
	{
		for(int i=0; i<wallPrecision; i++)
		{
			indices[(wallPrecision*j+i)*4+0]=i+(wallPrecision+1)*j;
			indices[(wallPrecision*j+i)*4+1]=i+1+(wallPrecision+1)*j;
			indices[(wallPrecision*j+i)*4+2]=i+1+(wallPrecision+1)*(j+1);
			indices[(wallPrecision*j+i)*4+3]=i+(wallPrecision+1)*(j+1);

			indices[(wallPrecision*j+i)*4+0+indiceswall1]=i+(wallPrecision+1)*j+index+1;
			indices[(wallPrecision*j+i)*4+1+indiceswall1]=i+1+(wallPrecision+1)*j+index+1;
			indices[(wallPrecision*j+i)*4+2+indiceswall1]=i+1+(wallPrecision+1)*(j+1)+index+1;
			indices[(wallPrecision*j+i)*4+3+indiceswall1]=i+(wallPrecision+1)*(j+1)+index+1;
			/*
			indices[((i+j*wallPrecision)*2)*3+0]=i+j*(wallPrecision+1);//0
			indices[((i+j*wallPrecision)*2)*3+1]=i+(j+1)*(wallPrecision+1);//3;
			indices[((i+j*wallPrecision)*2)*3+2]=i+1+j*(wallPrecision+1);//1
			indices[((i+j*wallPrecision)*2+1)*3+0]=i+1+j*(wallPrecision+1);//1
			indices[((i+j*wallPrecision)*2+1)*3+1]=i+(j+1)*(wallPrecision+1);//3
			indices[((i+j*wallPrecision)*2+1)*3+2]=i+1+(j+1)*(wallPrecision+1);//4*/
		}
	}

	//for model
	int i=1,j=1;
	indices[(wallPrecision*j+i)*4+0+indiceswall1]=0;
	indices[(wallPrecision*j+i)*4+1+indiceswall1]=0;
	indices[(wallPrecision*j+i)*4+2+indiceswall1]=0;
	indices[(wallPrecision*j+i)*4+3+indiceswall1]=0;
	
	return true;
}