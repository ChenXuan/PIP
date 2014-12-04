#include <windows.h>
#include <stdio.h>
#include <GL\gl.h>
#include "Asist/Maths.h"
#include "CEIL.h"

/////////////////////////////////////////////////////////////Perlin Noise
float persistence = 0.45f;
int Number_Of_Octaves = 3;

float Noise1(int x, int y)
{
	x = x % 25;
	y = y % 25;
	int n = x + y * 57;
	n = (n<<13) ^ n;
	return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f); 
}

float SmoothNoise_1(int x, int y)
{
	float corners = ( Noise1(x-1, y-1)+Noise1(x+1, y-1)+Noise1(x-1, y+1)+Noise1(x+1, y+1) ) / 16.0f;
	float sides   = ( Noise1(x-1, y)  +Noise1(x+1, y)  +Noise1(x, y-1)  +Noise1(x, y+1) ) /  8.0f;
	float center  =  Noise1(x, y) / 4.0f;
	return corners + sides + center;
}

float Cosine_Interpolate(float a, float b, float x)
{
	double ft = x * 3.1415927;
	double f = (1 - cos(ft)) * 0.5f;

	return  a*(1-f) + b*f;

}

float InterpolatedNoise_1(float x, float y)
{

	int integer_X    = int(x);
	float fractional_X = x - integer_X;

	int integer_Y    = int(y);
	float fractional_Y = y - integer_Y;

	float v1 = SmoothNoise_1(integer_X,     integer_Y);
	float v2 = SmoothNoise_1(integer_X + 1, integer_Y);
	float v3 = SmoothNoise_1(integer_X,     integer_Y + 1);
	float v4 = SmoothNoise_1(integer_X + 1, integer_Y + 1);

	float i1 = Cosine_Interpolate(v1 , v2 , fractional_X);
	float i2 = Cosine_Interpolate(v3 , v4 , fractional_X);

	return Cosine_Interpolate(i1 , i2 , fractional_Y);
}

float PerlinNoise_2D(float x, float y)
{
	float total = 0.0f;
	float p = persistence;
	int n = Number_Of_Octaves - 1;

	for(int i=0;i<=n;i++)
	{
		float frequency = pow((float)2,i);
		float amplitude = pow(p,i);

		total = total + InterpolatedNoise_1(x * frequency, y * frequency) * amplitude;
	}

	return total;
} 

CEIL::CEIL()
{

	InitCEIL();
}

CEIL::~CEIL()
{
	if(indices)
		delete [] indices;
	indices=NULL;

	if(vertices)
		delete [] vertices;
	vertices=NULL;
}

bool CEIL::InitCEIL()
{
	numVertices=(ceilPrecision+1)*(ceilPrecision+1);
	numIndices=4*ceilPrecision*ceilPrecision;

	vertices=new CEIL_VERTEX[numVertices];
	if(!vertices)
	{
		printf("Unable to allocate memory for ceil vertices\n");
		return false;
	}

	indices=new unsigned int[numIndices];
	if(!indices)
	{
		printf("Unable to allocate memory for ceil indices\n");
		return false;
	}

	int index=0;
	for(int i=0;i<ceilPrecision;i++){
		for(int j=0;j<ceilPrecision;j++)
		{
			index=j*(ceilPrecision+1)+i;
			if(i>5 && j>5 && i<ceilPrecision-5 && j<ceilPrecision-5)
			{
			vertices[index].position=VECTOR3D(0,0,0)+VECTOR3D((float)i*ceilWidth/ceilPrecision,
				-0.2f*PerlinNoise_2D((i+10000)/10.0f, (j+10000)/10.0f),-(float)j*ceilHeight/ceilPrecision);
			}
			else
				vertices[index].position=VECTOR3D(0,0,0)+VECTOR3D((float)i*ceilWidth/ceilPrecision,
				0,-(float)j*ceilHeight/ceilPrecision);
			vertices[index].t=(float)i/ceilPrecision;
			vertices[index].s=(float)j/ceilPrecision;
		}
	}
	for(int i=0;i<ceilPrecision;i++){
		for(int j=0;j<ceilPrecision;j++)
		{
			index=j*(ceilPrecision+1)+i;
			if(i>0 && j>0 && i<ceilPrecision-1 && j<ceilPrecision-1)//·¨Ïò¼ÆËã
			{
				VECTOR3D v1;
				v1.x=vertices[index+1].position.GetY() - vertices[index-1].position.GetY();
				v1.y= 0.5f;
				v1.z=vertices[index+ceilPrecision+1].position.y - vertices[index-ceilPrecision-1].position.y;
				v1.Normalize();
				vertices[index].normal.Set(v1.x,v1.y,v1.z);
			}
			else
			{
				vertices[index].normal.Set(0,1,0);
			}
		}
	}
}
