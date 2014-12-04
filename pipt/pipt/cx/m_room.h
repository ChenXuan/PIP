#include "Asist/Maths.h"
class MRoom{
public:
	void paint(GLfloat*,GLfloat*);
private:
	void initw();
	void initTexture();
	void drawwall(GLfloat*,GLfloat*);
	void drawfloor(GLfloat*,GLfloat*);
	void drawceil(GLfloat*);
};