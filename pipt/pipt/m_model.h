static float M_WIDTH=3.0f;
class MModel{

public:

	void paint()
	{

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(0.5f, 2.0f, -2.0f);

		glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0,0,-M_WIDTH);
		glVertex3f(0,M_WIDTH,-M_WIDTH);
		glVertex3f(0,M_WIDTH,0);
		glVertex3f(0,0,0);
		glEnd();
		glPopMatrix();
	};
};