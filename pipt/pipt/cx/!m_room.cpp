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

//#include "OBJParser.h"

#define BMP_Header_Length 54

static GLfloat ROOM=6.0f;
//GLuint texGround;

WALL wall;
WALL myfloor;

bool drawBumps=true;
bool drawColor=true;

bool isInited=false;

//Normal map
GLuint normalMap1;
GLuint normalMap2;


//Decal texture
GLuint decalTexture1;
GLuint decalTexture2;


//Normalisation cube map
GLuint normalisationCubeMap1;
GLuint normalisationCubeMap2;


IMAGE normalMapImage1;
IMAGE normalMapImage2;


IMAGE decalImage1;
IMAGE decalImage2;

void MRoom::paint(GLdouble* eye,GLdouble* light){ 
	if(!isInited){
		initw();
	}

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();

	gluLookAt(	eye[0],eye[1],eye[2], 
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);


	/*rotate
	static float angle=0.0f;
	angle+=0.1f;*/
	//glRotatef(0, 0.0f, 1.0f, 0.0f);
	
	//Get the inverse model matrix
	MATRIX4X4 inverseModelMatrix;
	glPushMatrix();
	glLoadIdentity();
	//glRotatef(0, 0.0f, 1.0f, 0.0f);
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
		glBindTexture(GL_TEXTURE_2D, normalMap1);
		glEnable(GL_TEXTURE_2D);

		//Bind normalisation cube map to texture unit 1
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, normalisationCubeMap1);
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
		glDrawElements(GL_TRIANGLES, wall.numIndices, GL_UNSIGNED_INT, wall.indices);


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
		glBindTexture(GL_TEXTURE_2D, decalTexture1);
		glEnable(GL_TEXTURE_2D);

		//Set vertex arrays for wall
		glVertexPointer(3, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].position);
		glEnableClientState(GL_VERTEX_ARRAY);

		glNormalPointer(GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].normal);
		glEnableClientState(GL_NORMAL_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].s);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Draw wall
		glDrawElements(GL_TRIANGLES, wall.numIndices, GL_UNSIGNED_INT, wall.indices);

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

	glFinish();
	glPopMatrix();
	//glFlush();

	//drawwall(eye,light);
	//drawthesecondwall(eye,light);
	//drawfloor(eye,light);

	//Disable blending if it is enabled
	//if(drawBumps && drawColor)
		//glDisable(GL_BLEND);

	//glFinish();
	//glutSwapBuffers();
}

void MRoom::initw(){
		
		if(	!SetUpARB_multitexture()		||	!SetUpARB_texture_cube_map()	||
			!SetUpARB_texture_env_combine()	||	!SetUpARB_texture_env_dot3())
		{
			printf("Required Extension Unsupported\n");
			exit(0);
		}
		/*
		//Load identity modelview
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();*/
		
		//Shading states
		glShadeModel(GL_SMOOTH);
		//glClearColor(0.2f, 0.4f, 0.2f, 0.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		
		//Depth states
		//glClearDepth(1.0f);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		
		initTexture();
		isInited=true;
}




void MRoom::initTexture(){

	//////////////////////////////////////////////////////////////////////////Texture for wall1
	//Load normal map
	
	normalMapImage1.Load(".\\cx\\Textures\\Normal map1.bmp");
	normalMapImage1.ExpandPalette();

	//Convert normal map to texture
	glGenTextures(1, &normalMap1);
	glBindTexture(GL_TEXTURE_2D, normalMap1);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGB8, normalMapImage1.width, normalMapImage1.height,
		0, normalMapImage1.format, GL_UNSIGNED_BYTE, normalMapImage1.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Load decal image
	decalImage1.Load(".\\cx\\Textures\\decal1.bmp");
	decalImage1.ExpandPalette();

	//Convert decal image to texture
	glGenTextures(1, &decalTexture1);
	glBindTexture(GL_TEXTURE_2D, decalTexture1);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGBA8, decalImage1.width, decalImage1.height,
		0, decalImage1.format, GL_UNSIGNED_BYTE, decalImage1.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Create normalisation cube map
	glGenTextures(1, &normalisationCubeMap1);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, normalisationCubeMap1);
	GenerateNormalisationCubeMap();
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//////////////////////////////////////////////////////////////////////////texture for wall 2
	//Load normal map
	normalMapImage2.Load(".\\cx\\Textures\\Normal map2.bmp");
	normalMapImage2.ExpandPalette();

	//Convert normal map to texture
	glGenTextures(1, &normalMap2);
	glBindTexture(GL_TEXTURE_2D, normalMap2);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGB8, normalMapImage2.width, normalMapImage2.height,
		0, normalMapImage2.format, GL_UNSIGNED_BYTE, normalMapImage2.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Load decal image
	decalImage2.Load(".\\cx\\Textures\\decal2.bmp");
	decalImage2.ExpandPalette();

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
	glPopMatrix();
}

void MRoom::drawwall(GLdouble* eye,GLdouble* light){
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();

	gluLookAt(	eye[0],eye[1],eye[2], 
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);


	/*rotate
	static float angle=0.0f;
	angle+=0.1f;*/
	//glRotatef(0, 0.0f, 1.0f, 0.0f);
	
	//Get the inverse model matrix
	MATRIX4X4 inverseModelMatrix;
	glPushMatrix();
	glLoadIdentity();
	//glRotatef(0, 0.0f, 1.0f, 0.0f);
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
		glBindTexture(GL_TEXTURE_2D, normalMap1);
		glEnable(GL_TEXTURE_2D);

		//Bind normalisation cube map to texture unit 1
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, normalisationCubeMap1);
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
		glDrawElements(GL_TRIANGLES, wall.numIndices, GL_UNSIGNED_INT, wall.indices);


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
		glBindTexture(GL_TEXTURE_2D, decalTexture1);
		glEnable(GL_TEXTURE_2D);

		//Set vertex arrays for wall
		glVertexPointer(3, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].position);
		glEnableClientState(GL_VERTEX_ARRAY);

		glNormalPointer(GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].normal);
		glEnableClientState(GL_NORMAL_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, sizeof(WALL_VERTEX), &wall.vertices[0].s);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Draw wall
		glDrawElements(GL_TRIANGLES, wall.numIndices, GL_UNSIGNED_INT, wall.indices);

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

	glFinish();
	glPopMatrix();
	//glFlush();
}

void MRoom::drawfloor(GLdouble* eye,GLdouble* light){
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();
	
	gluLookAt(	eye[0],eye[1],eye[2], 
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	/*rotate
	static float angle=0.0f;
	angle+=0.1f;
	
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	*/
	//glTranslatef(0,2,0);
	//Get the inverse model matrix
	MATRIX4X4 inverseModelMatrix;
	glPushMatrix();
	glLoadIdentity();
	//glTranslatef(0,-2,0);
	//glRotatef(-90, 1.0f, 0.0f, 0.0f);
	
	glGetFloatv(GL_MODELVIEW_MATRIX, inverseModelMatrix);
	glPopMatrix();

	VECTOR3D worldLightPosition3=VECTOR3D(light[0], light[1], light[2]);
	//Get the object space light vector
	VECTOR3D objectLightPosition=inverseModelMatrix*worldLightPosition3;

	//Loop through vertices
	for(int i=0; i<myfloor.numVertices; ++i)
	{
		VECTOR3D lightVector=objectLightPosition-myfloor.vertices[i].position;

		//Calculate tangent space light vector
		myfloor.vertices[i].tangentSpaceLight.x=
			myfloor.vertices[i].sTangent.DotProduct(lightVector);
		myfloor.vertices[i].tangentSpaceLight.y=
			myfloor.vertices[i].tTangent.DotProduct(lightVector);
		myfloor.vertices[i].tangentSpaceLight.z=
			myfloor.vertices[i].normal.DotProduct(lightVector);
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

		
		//Set vertex arrays for floor
		glVertexPointer(3, GL_FLOAT, sizeof(WALL_VERTEX),&myfloor.vertices[0].position);//
		glEnableClientState(GL_VERTEX_ARRAY);

		//Send texture coords for normal map to unit 0
		glTexCoordPointer(2, GL_FLOAT, sizeof(WALL_VERTEX), &myfloor.vertices[0].s);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Send tangent space light vectors for normalisation to unit 1
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glTexCoordPointer(3, GL_FLOAT, sizeof(WALL_VERTEX), &myfloor.vertices[0].tangentSpaceLight);
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


		//Draw floor
		glDrawElements(GL_QUADS, myfloor.numIndices, GL_UNSIGNED_INT, myfloor.indices);


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
		glVertexPointer(3, GL_FLOAT, sizeof(WALL_VERTEX), &myfloor.vertices[0].position);
		glEnableClientState(GL_VERTEX_ARRAY);

		glNormalPointer(GL_FLOAT, sizeof(WALL_VERTEX), &myfloor.vertices[0].normal);
		glEnableClientState(GL_NORMAL_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, sizeof(WALL_VERTEX), &myfloor.vertices[0].s);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Draw wall
		glDrawElements(GL_QUADS, myfloor.numIndices, GL_UNSIGNED_INT, myfloor.indices);

		if(!drawBumps)
			glDisable(GL_LIGHTING);

		//Disable texture
		glDisable(GL_TEXTURE_2D);

		//disable vertex arrays
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	}
	glPopMatrix();

}
