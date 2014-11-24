static float P_WIDTH=3.0f;
class MPicture{

public:
	
	void paint()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(2.0f, 2.0f, -0.5f);

		glBegin(GL_POLYGON);
		glColor3f(0,0,0);
		glVertex3f(P_WIDTH,P_WIDTH,0);
		glVertex3f(0,P_WIDTH,0);
		glVertex3f(0,0,0);
		glVertex3f(P_WIDTH,0,0);
		glEnd();
		glPopMatrix();
	};
};