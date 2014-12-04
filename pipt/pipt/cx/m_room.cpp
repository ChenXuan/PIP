#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include "m_room.h"
#include "Extensions/ARB_multitexture_extension.h"
#include "Extensions/ARB_texture_cube_map_extension.h"
#include "Extensions/ARB_texture_env_combine_extension.h"
#include "Extensions/ARB_texture_env_dot3_extension.h"
#include "Maths/Maths.h"
#include "Image/IMAGE.h"
#include "Normalisation Cube Map.h"
#include "WALL.h"
#include "CEIL.h"

#define BMP_Header_Length 54

//Our wall
WALL wall;

//Normal map
GLuint normalMap;
GLuint normalMap2;

//Decal texture
GLuint decalTexture;
GLuint decalTexture2;
GLuint ceilTexture;

//Normalisation cube map
GLuint normalisationCubeMap;
GLuint normalisationCubeMap2;

bool drawBumps=true;
bool drawColor=true;
bool isInited=false;
//

void MRoom::paint(GLfloat* eye,GLfloat* light){ 
	initw();
	drawwall(eye,light);
	drawceil(light);
}

IMAGE normalMapImage;
IMAGE decalImage;
IMAGE normalMapImage2;
IMAGE decalImage2;
IMAGE ceilImage;
//////////////////////////////////////////////////////////////////////////bump mapping part
void MRoom::initw(){
	if(!isInited){
		//Check for and set up extensions
		if(	!SetUpARB_multitexture()		||	!SetUpARB_texture_cube_map()	||
			!SetUpARB_texture_env_combine()	||	!SetUpARB_texture_env_dot3())
		{
			printf("Required Extension Unsupported\n");
			exit(0);
		}

		//Load normal map
		normalMapImage.Load(".\\cx\\Textures\\Normal map1.bmp");
		normalMapImage.ExpandPalette();

		//Load decal image
		decalImage.Load(".\\cx\\Textures\\Decal1.bmp");
		decalImage.ExpandPalette();

		//Load normal map

		normalMapImage2.Load(".\\cx\\Textures\\Normal map2.bmp");
		normalMapImage2.ExpandPalette();

		//Load decal image

		decalImage2.Load(".\\cx\\Textures\\decal2.bmp");
		decalImage2.ExpandPalette();
		//initTexture();

		ceilImage.Load(".\\cx\\Textures\\decal4.bmp");
        ceilImage.ExpandPalette();
	}
	
	

	//Load identity modelview
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	//Shading states
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.4f, 0.2f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//Depth states
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_CULL_FACE);

	//Convert normal map to texture
	glGenTextures(1, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGBA8, normalMapImage.width, normalMapImage.height,
		0, normalMapImage.format, GL_UNSIGNED_BYTE, normalMapImage.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Convert decal image to texture
	glGenTextures(1, &decalTexture);
	glBindTexture(GL_TEXTURE_2D, decalTexture);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGBA8, decalImage.width, decalImage.height,
		0, decalImage.format, GL_UNSIGNED_BYTE, decalImage.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//Create normalisation cube map
	glGenTextures(1, &normalisationCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, normalisationCubeMap);
	GenerateNormalisationCubeMap();
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	
	//Convert normal map to texture
	glGenTextures(1, &normalMap2);
	glBindTexture(GL_TEXTURE_2D, normalMap2);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGBA8, normalMapImage2.width, normalMapImage2.height,
		0, normalMapImage2.format, GL_UNSIGNED_BYTE, normalMapImage2.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Convert decal image to texture
	glGenTextures(1, &decalTexture2);
	glBindTexture(GL_TEXTURE_2D, decalTexture2);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGBA8, decalImage2.width, decalImage2.height,
		0, decalImage2.format, GL_UNSIGNED_BYTE, decalImage2.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Create normalisation cube map
	glGenTextures(1, &normalisationCubeMap2);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, normalisationCubeMap2);
	GenerateNormalisationCubeMap();
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//Convert decal image to texture
	glGenTextures(1, &ceilTexture);
	glBindTexture(GL_TEXTURE_2D, ceilTexture);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGBA8, ceilImage.width, ceilImage.height,
		0, ceilImage.format, GL_UNSIGNED_BYTE, ceilImage.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//isInited=true;
}

void MRoom::drawwall(GLfloat* eye,GLfloat* light){
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();

	//use gluLookAt to look at wall
	gluLookAt(	eye[0],eye[1],eye[2], 
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	//rotate wall
	//static float angle=0.0f;
	//angle+=0.1f;
	//glRotatef(angle, 0.0f, 1.0f, 0.0f);


	//Get the inverse model matrix
	MATRIX4X4 inverseModelMatrix;
	glPushMatrix();
	glLoadIdentity();
	//glRotatef(-angle, 0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, inverseModelMatrix);
	glPopMatrix();

	//Light position in world space
	VECTOR3D worldLightPosition=VECTOR3D(light[0],light[1],light[2]);
	//Get the object space light vector
	VECTOR3D objectLightPosition=inverseModelMatrix*worldLightPosition;

	//Loop through vertices
	for(int i=0; i<wall.numVertices; ++i)
	{
		VECTOR3D lightVector=objectLightPosition-wall.vertices[i].position;

		//Calculate tangent space light vector
		wall.vertices[i].tangentSpaceLight.x=
			wall.vertices[i].sTangent.DotProduct(lightVector);
		wall.vertices[i].tangentSpaceLight.y=
			wall.vertices[i].tTangent.DotProduct(lightVector);
		wall.vertices[i].tangentSpaceLight.z=
			wall.vertices[i].normal.DotProduct(lightVector);
	}


	//Draw bump pass
	if(drawBumps)
	{
		//Bind normal map to texture unit 0
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glEnable(GL_TEXTURE_2D);

		//Bind normalisation cube map to texture unit 1
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, normalisationCubeMap);
		glEnable(GL_TEXTURE_CUBE_MAP_ARB);
		glActiveTextureARB(GL_TEXTURE0_ARB);

		//Set vertex arrays for wall
		glVertexPointer(3, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].position);
		glEnableClientState(GL_VERTEX_ARRAY);

		//Send texture coords for normal map to unit 0
		glTexCoordPointer(2, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].s);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Send tangent space light vectors for normalisation to unit 1
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glTexCoordPointer(3, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].tangentSpaceLight);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTextureARB(GL_TEXTURE0_ARB);


		//Set up texture environment to do (tex0 dot tex1)*color
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);

		glActiveTextureARB(GL_TEXTURE1_ARB);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);

		glActiveTextureARB(GL_TEXTURE0_ARB);



		//Draw wall
		glDrawElements(GL_QUADS, wall.numIndices, GL_UNSIGNED_INT, wall.indices);


		//Disable textures
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glActiveTextureARB(GL_TEXTURE0_ARB);

		//disable vertex arrays
		glDisableClientState(GL_VERTEX_ARRAY);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTextureARB(GL_TEXTURE0_ARB);

		//Return to standard modulate texenv
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	//If we are drawing both passes, enable blending to multiply them together
	if(drawBumps && drawColor)
	{
		//Enable multiplicative blending
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glEnable(GL_BLEND);
	}

	//Perform a second pass to color the wall
	if(drawColor)
	{
		if(!drawBumps)
		{
			glLightfv(GL_LIGHT1, GL_POSITION, VECTOR4D(objectLightPosition));
			glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
			glLightfv(GL_LIGHT1, GL_AMBIENT, black);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
			glEnable(GL_LIGHT1);
			glEnable(GL_LIGHTING);

			glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
		}

		//Bind decal texture
		glBindTexture(GL_TEXTURE_2D, decalTexture);
		glEnable(GL_TEXTURE_2D);

		//Set vertex arrays for wall
		glVertexPointer(3, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].position);
		glEnableClientState(GL_VERTEX_ARRAY);

		glNormalPointer(GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].normal);
		glEnableClientState(GL_NORMAL_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].s);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Draw wall
		glDrawElements(GL_QUADS, wall.numIndices, GL_UNSIGNED_INT, wall.indices);

		if(!drawBumps)
			glDisable(GL_LIGHTING);

		//Disable texture
		glDisable(GL_TEXTURE_2D);

		//disable vertex arrays
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	//Disable blending if it is enabled
	if(drawBumps && drawColor)
		glDisable(GL_BLEND);

	drawfloor(eye,light);
	glPopMatrix();
	glFinish();
}

void MRoom::drawfloor(GLfloat* eye,GLfloat* light){
	{
		glPushMatrix();
		glLoadIdentity();

		//use gluLookAt to look at wall
		gluLookAt(	eye[0],eye[1],eye[2], 
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f);

		//rotate wall
		glTranslatef(0,0,-15);
		glRotatef(90, 1.0f, 0.0f, 0.0f);
		//glRotatef(90,0,0,1);
		


		//Get the inverse model matrix
		MATRIX4X4 inverseModelMatrix;
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(0,0,15);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		
		//glRotatef(-90,0,0,1);
		glGetFloatv(GL_MODELVIEW_MATRIX, inverseModelMatrix);
		glPopMatrix();

		//Light position in world space
		VECTOR3D worldLightPosition=VECTOR3D(light[0],light[1],light[2]);
		//Get the object space light vector
		VECTOR3D objectLightPosition=inverseModelMatrix*worldLightPosition;

		//Loop through vertices
		for(int i=0; i<wall.numVertices; ++i)
		{
			VECTOR3D lightVector=objectLightPosition-wall.vertices[i].position;

			//Calculate tangent space light vector
			wall.vertices[i].tangentSpaceLight.x=
				wall.vertices[i].sTangent.DotProduct(lightVector);
			wall.vertices[i].tangentSpaceLight.y=
				wall.vertices[i].tTangent.DotProduct(lightVector);
			wall.vertices[i].tangentSpaceLight.z=
				wall.vertices[i].normal.DotProduct(lightVector);
		}


		//Draw bump pass
		if(drawBumps)
		{
			//Bind normal map to texture unit 0
			glBindTexture(GL_TEXTURE_2D, normalMap2);
			glEnable(GL_TEXTURE_2D);

			//Bind normalisation cube map to texture unit 1
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, normalisationCubeMap2);
			glEnable(GL_TEXTURE_CUBE_MAP_ARB);
			glActiveTextureARB(GL_TEXTURE0_ARB);

			//Set vertex arrays for wall
			glVertexPointer(3, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].position);
			glEnableClientState(GL_VERTEX_ARRAY);

			//Send texture coords for normal map to unit 0
			glTexCoordPointer(2, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].s);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			//Send tangent space light vectors for normalisation to unit 1
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glTexCoordPointer(3, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].tangentSpaceLight);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glClientActiveTextureARB(GL_TEXTURE0_ARB);


			//Set up texture environment to do (tex0 dot tex1)*color
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);

			glActiveTextureARB(GL_TEXTURE1_ARB);

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);

			glActiveTextureARB(GL_TEXTURE0_ARB);



			//Draw wall
			glDrawElements(GL_QUADS, wall.numIndices/2, GL_UNSIGNED_INT, wall.indices);


			//Disable textures
			glDisable(GL_TEXTURE_2D);

			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_CUBE_MAP_ARB);
			glActiveTextureARB(GL_TEXTURE0_ARB);

			//disable vertex arrays
			glDisableClientState(GL_VERTEX_ARRAY);

			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glClientActiveTextureARB(GL_TEXTURE0_ARB);

			//Return to standard modulate texenv
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		}

		//If we are drawing both passes, enable blending to multiply them together
		if(drawBumps && drawColor)
		{
			//Enable multiplicative blending
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			glEnable(GL_BLEND);
		}

		//Perform a second pass to color the wall
		if(drawColor)
		{
			if(!drawBumps)
			{
				glLightfv(GL_LIGHT1, GL_POSITION, VECTOR4D(objectLightPosition));
				glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
				glLightfv(GL_LIGHT1, GL_AMBIENT, black);
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
				glEnable(GL_LIGHT1);
				glEnable(GL_LIGHTING);

				glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
			}

			//Bind decal texture
			glBindTexture(GL_TEXTURE_2D, decalTexture2);
			glEnable(GL_TEXTURE_2D);

			//Set vertex arrays for wall
			glVertexPointer(3, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].position);
			glEnableClientState(GL_VERTEX_ARRAY);

			glNormalPointer(GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].normal);
			glEnableClientState(GL_NORMAL_ARRAY);

			glTexCoordPointer(2, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].s);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			//Draw wall
			glDrawElements(GL_QUADS, wall.numIndices/2, GL_UNSIGNED_INT, wall.indices);

			if(!drawBumps)
				glDisable(GL_LIGHTING);

			//Disable texture
			glDisable(GL_TEXTURE_2D);

			//disable vertex arrays
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		//Disable blending if it is enabled
		if(drawBumps && drawColor)
			glDisable(GL_BLEND);
		glPopMatrix();

	}
}

//////////////////////////////////////////////////////////////////////////perlin noise part
CEIL myceil;
void MRoom::drawceil(GLfloat* light){
	 
	GLfloat sun_light_ambient[]   = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat sun_light_diffuse[]   = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat sun_light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, light);
	glLightfv(GL_LIGHT0, GL_AMBIENT,   sun_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,   sun_light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glRotatef(90,1,0,0);
	glTranslatef(0, -15,0);
	

	glBindTexture(GL_TEXTURE_2D, ceilTexture);
	//glBindTexture(GL_TEXTURE_2D, 0);
	int index,index2;
	for(int j=0;j<ceilPrecision-1;j++)
	{
		glBegin(GL_QUAD_STRIP);
		for(int i=0;i<ceilPrecision;i++)
		{
			index=j*(ceilPrecision+1)+i;
			index2=index+ceilPrecision+1;
			glNormal3f(myceil.vertices[index].normal.x ,myceil.vertices[index].normal.y , myceil.vertices[index].normal.z); 
			glTexCoord2f(myceil.vertices[index].s, myceil.vertices[index].t);
			glVertex3f(myceil.vertices[index].position.x, myceil.vertices[index].position.y, myceil.vertices[index].position.z);
			//glVertex3f(i,j,-1);
			glNormal3f(myceil.vertices[index2].normal.x ,myceil.vertices[index2].normal.y , myceil.vertices[index2].normal.z); 
			glTexCoord2f(myceil.vertices[index2].s, myceil.vertices[index2].t);
			glVertex3f(myceil.vertices[index2].position.x, myceil.vertices[index2].position.y, myceil.vertices[index2].position.z);
			//glVertex3f(i,j+1,-1);
		}
		glEnd();
	}
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glPopMatrix();
}
