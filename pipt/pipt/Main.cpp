#include <gl/glut.h>
#include <stdlib.h>
#include "m_room.h"
#include "m_picture.h"
#include "m_model.h"
#include "m_shader.h"
#include "m_frame.h"

#define WIDTH 1024
#define HEIGHT 768
#define X 0
#define Y 1
#define Z 2
#define NEAR 1
#define FAR 100


static GLfloat angle = 0.0f;

static GLdouble eye[]={5.0, 5.0, -10.0};

MRoom* room;
MPicture* pic;
MModel* model;
MShader* shader;
MFrame* frame;
void myDisplay(void)
{
	//room=new Room();
	//pic=new Picture();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// 创建透视效果视图
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, 1.0f, NEAR, FAR);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[X],eye[Y],eye[Z], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	

	pic->paint();
	room->paint();
	model->paint();
	shader->paint();
	frame->paint();
	glutSwapBuffers();
}


/* mouse callback */
void mouse(int btn, int state, int x, int y)
{
	/* select an axis about which to rotate */

}

double step=0.5;
/* keyboard callback */
void keyboard(unsigned char key, int x, int y)
{
	switch(key){
	case 'w':
		eye[Z]+=step;
		break;
	case 's':
		eye[Z]-=step;
		break;
	case 'a':
		eye[X]-=step;
		break;
	case 'd':
		eye[X]+=step;
		break;
	case 'q':
		exit(0);
	}
	myDisplay();
}

/* reshape callback */
void myReshape(int w, int h)
{
}
void myIdle(void)
{
	
}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL光照演示");
	glutDisplayFunc(&myDisplay);
	//glutIdleFunc(&myIdle);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}