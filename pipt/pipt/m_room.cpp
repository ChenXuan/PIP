#include <stdlib.h>
#include <gl/glut.h>
#include "m_room.h"
static GLfloat ROOM=6.0f;
void MRoom::paint(){
	int x=0;
	{// room
		glBegin(GL_POLYGON);
		glColor3f(ROOM, ROOM, 0.0f);
		glVertex3f(0,0,0);
		glVertex3f(ROOM,0,0);
		glVertex3f(ROOM,0,-ROOM);
		glVertex3f(0,0,-ROOM);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(ROOM,ROOM,0);
		glVertex3f(0,ROOM,0);
		glVertex3f(0,0,0);
		glVertex3f(ROOM,0,0);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(0.0f, 1.0f, .0f);
		glVertex3f(0,0,-ROOM);
		glVertex3f(0,ROOM,-ROOM);
		glVertex3f(0,ROOM,0);
		glVertex3f(0,0,0);
		glEnd();
	}
}