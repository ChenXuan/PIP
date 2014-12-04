#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\glut.h>
#include <malloc.h>
static float M_WIDTH=3.0f;

#define BMP_Header_Length 54
#define LENGTH 5
#define SIGMA 3
float PI = 3.1415926535898;
int r = 0;
int rstep = 6;

////////////////////////////////////////////////////////////////////////////



typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

BYTE l_red, l_green, l_blue;

typedef struct tagBITMAPINFOHEADER{
	DWORD biSize;
	long   biWidth;
	long   biHeight;
	WORD   biPlanes;
	WORD   biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	long   biXPelsPerMeter;
	long   biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BITMAPINFOHEADER;

int ReadBmp(const char* szFileName);
int GetDIBColor(int X, int Y, BYTE *r, BYTE *g, BYTE *b);

BITMAPINFOHEADER bih;
BYTE *Buffer = NULL;
long LineByteWidth;

int ReadBmp(const char* szFileName)
{
	FILE *file;
	WORD bfh[7];
	long dpixeladd;

	if (NULL == (file = fopen(szFileName, "rb")))
	{
		return 0;
	}
	//printf("%s\n", szFileName);

	fread(&bfh, sizeof(WORD), 7, file);
	if (bfh[0] != (WORD)(((WORD)'B') | ('M' << 8)))
	{
		fclose(file);
		return 0;
	}

	fread(&bih, sizeof(BITMAPINFOHEADER), 1, file);

	if (bih.biBitCount < 24)
	{
		fclose(file);
		return 0;
	}

	dpixeladd = bih.biBitCount / 8;
	LineByteWidth = bih.biWidth * (dpixeladd);
	if ((LineByteWidth % 4) != 0)
		LineByteWidth += 4 - (LineByteWidth % 4);

	if ((Buffer = (BYTE*)malloc(sizeof(BYTE)* LineByteWidth * bih.biHeight)) != NULL)
	{
		fread(Buffer, LineByteWidth * bih.biHeight, 1, file);

		fclose(file);
		return 1;
	}

	fclose(file);
	return 0;
}

int GetDIBColor(int X, int Y, BYTE *r, BYTE *g, BYTE *b)
{
	int dpixeladd;
	BYTE *ptr;
	if (X < 0 || X >= bih.biWidth || Y < 0 || Y >= bih.biHeight)
	{
		return 0;
	}

	dpixeladd = bih.biBitCount / 8;
	ptr = Buffer + X * dpixeladd + (bih.biHeight - 1 - Y) * LineByteWidth;

	*b = *ptr;
	*g = *(ptr + 1);
	*r = *(ptr + 2);

	return 1;
}

void ReadRGB()
{
	char szfilename[255] = "Worldmap.bmp";
	ReadBmp(szfilename);
	//printf("Width: %ld\n", bih.biWidth);
	//printf("Height: %ld\n", bih.biHeight);
	//printf("BitCount: %d\n\n", (int)bih.biBitCount);
}

////////////////////////////////////////////////////////////////////////////


void TimerFunction(int value)
{
	r = (r + rstep) % 360;
	glutPostRedisplay();
}


class MModel{

public:
	
	
	int power_of_two(int n)
	{
		if (n <= 0)
			return 0;
		return (n & (n - 1)) == 0;
	}

	GLuint load_texture(const char* file_name)
	{
		GLint width, height, total_bytes;
		GLubyte* pixels = 0;
		GLuint last_texture_ID, texture_ID = 0;
		// 打开文件，如果失败，返回
		FILE* pFile = fopen(file_name, "rb");
		if (pFile == 0)
			return 0;
		// 读取文件中图象的宽度和高度
		fseek(pFile, 0x0012, SEEK_SET);
		fread(&width, 4, 1, pFile);
		fread(&height, 4, 1, pFile);
		fseek(pFile, BMP_Header_Length, SEEK_SET);
		// 计算每行像素所占字节数，并根据此数据计算总像素字节数
		{
			GLint line_bytes = width * 3;
			while (line_bytes % 4 != 0)
				++line_bytes;
			total_bytes = line_bytes * height;
		}
		// 根据总像素字节数分配内存
		pixels = (GLubyte*)malloc(total_bytes);
		if (pixels == 0)
		{
			fclose(pFile);
			return 0;
		}
		// 读取像素数据
		if (fread(pixels, total_bytes, 1, pFile) <= 0)
		{
			free(pixels);
			fclose(pFile);
			return 0;
		}
		// 分配一个新的纹理编号
		glGenTextures(1, &texture_ID);
		if (texture_ID == 0)
		{
			free(pixels);
			fclose(pFile);
			return 0;
		}
		// 绑定新的纹理，载入纹理并设置纹理参数
		// 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
		glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&last_texture_ID);
		glBindTexture(GL_TEXTURE_2D, texture_ID);
		//纹理过滤设定
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//颜色混合模式
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
			GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
		glBindTexture(GL_TEXTURE_2D, last_texture_ID);
		// 之前为pixels分配的内存可在使用glTexImage2D以后释放
		// 因为此时像素数据已经被OpenGL另行保存了一份（可能被保存到专门的图形硬件中）
		free(pixels);
		return texture_ID;
	}

	

	void paintBackground()
	{

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(0.6f, 2.0f, -2.0f);
		//cx
		glTranslatef(-0.6,3,-3);
		glScaled(1.66,1.66,1.66);

		glEnable(GL_TEXTURE_2D);
		GLuint texGround = load_texture("Star.bmp");
		glBindTexture(GL_TEXTURE_2D, texGround);

		double x_in = -0.5;
		
		glBegin(GL_POLYGON);
		//glColor3f(1, 1, 1);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0, M_WIDTH, 0);
		glTexCoord2f(0.25f, 0.75f); glVertex3f(x_in, M_WIDTH, 0);
		glTexCoord2f(0.75f, 0.75f); glVertex3f(x_in, M_WIDTH, -M_WIDTH);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0, M_WIDTH, -M_WIDTH);
		glEnd();

		glBegin(GL_POLYGON);
		//glColor3f(1, 1, 1);	
		glTexCoord2f(0.25f, 0.25f); glVertex3f(x_in, 0, 0);
		glTexCoord2f(0.75f, 0.25f); glVertex3f(x_in, 0, -M_WIDTH);
		glTexCoord2f(0.75f, 0.75f); glVertex3f(x_in, M_WIDTH, -M_WIDTH);
		glTexCoord2f(0.25f, 0.75f); glVertex3f(x_in, M_WIDTH, 0);
		glEnd();


		
		glBegin(GL_POLYGON);
		//glColor3f(1, 1, 1);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 0);
		glTexCoord2f(1.0f, 0.0f);  glVertex3f(0, 0, -M_WIDTH);
		glTexCoord2f(0.75f, 0.25f); glVertex3f(x_in, 0, -M_WIDTH);
		glTexCoord2f(0.25f, 0.25f); glVertex3f(x_in, 0, 0);
		glEnd();
		
		
		glBegin(GL_POLYGON);
		//glColor3f(1, 1, 1);
		glTexCoord2f(0.75f, 0.25f); glVertex3f(x_in, 0, -M_WIDTH);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0, 0, -M_WIDTH);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0, M_WIDTH, -M_WIDTH);
		glTexCoord2f(0.75f, 0.75f); glVertex3f(x_in, M_WIDTH, -M_WIDTH);
		glEnd();
		

		
		glBegin(GL_POLYGON);
		//glColor3f(1, 1, 1);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0, M_WIDTH,0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 0);
		glTexCoord2f(0.25f, 0.25f); glVertex3f(x_in, 0, 0);
		glTexCoord2f(0.25f, 0.75f); glVertex3f(x_in, M_WIDTH, 0);
		glEnd();
		
		

		glDisable(GL_TEXTURE_2D);


		
	};
	float CalculateOffset(int R, int G, int B)
	{
		if (R <G &&G<B)
			return 0.96f;

		else
			return 1.02f;
	}
	void paintGlobal()
	{
		glMatrixMode(GL_SMOOTH);
		glPushMatrix();
		//cx
		glTranslatef(-0.75f, 1.7f, -1.6f);
		glScaled(0.4f, 0.4f, 0.4f);
		//
		glEnable(GL_TEXTURE_2D);
		GLuint texGround = load_texture("Worldmap.bmp");
		glBindTexture(GL_TEXTURE_2D, texGround);
		

		float Radium = 2;

		glBegin(GL_POLYGON);
		float x_rate, y_rate;
		int X, Y;
		int R, G, B;
		float offset = 1.0f;
		for (int i = 0; i < 360; i++)
		{
			for (int j = 0; j < 180; j++)
			{
				
				//glColor3f(1, 1, 1);
				
				x_rate = (i + r) % 360 / 360.0;
				y_rate = 1 - j / 180.0;
				X = x_rate*(bih.biWidth-1);
				Y = y_rate*(bih.biHeight-1);
				GetDIBColor(X, Y, &l_red, &l_green, &l_blue);
				R = l_red;
				G = l_green;
				B = l_blue;
				offset = CalculateOffset(R, G, B);

				glTexCoord2f((i + r) / 360.0, 1 - j / 180.0);
				glVertex3f(Radium * sin(j / 180.0*PI)*cos(i / 180.0*PI) * offset,
					Radium*cos(j / 180.0*PI)* offset,
					-Radium*sin(j / 180.0*PI)*sin(i / 180.0*PI) * offset
					);

				x_rate = (i + r) % 360 / 360.0;
				y_rate = 1 - (j + 1) / 180.0;
				X = x_rate*(bih.biWidth-1);
				Y = y_rate*(bih.biHeight-1);
				GetDIBColor(X, Y, &l_red, &l_green, &l_blue);
				R = l_red;
				G = l_green;
				B = l_blue;
				offset = CalculateOffset(R, G, B);
				glTexCoord2f((i + r) / 360.0, 1 - (j + 1) / 180.0);
				glVertex3f(Radium * sin((j + 1) / 180.0*PI)*cos(i / 180.0*PI) * offset,
					Radium*cos((j + 1) / 180.0*PI) * offset,
					-Radium*sin((j + 1) / 180.0*PI)*sin(i / 180.0*PI) * offset
					);

				x_rate = (i + r + 1) % 360 / 360.0;
				y_rate = 1 - (j + 1) / 180.0;
				X = x_rate*(bih.biWidth-1);
				Y = y_rate*(bih.biHeight-1);
				GetDIBColor(X, Y, &l_red, &l_green, &l_blue);
				R = l_red;
				G = l_green;
				B = l_blue;
				offset = CalculateOffset(R, G, B);
				glTexCoord2f((i + r + 1) / 360.0, 1 - (j + 1) / 180.0);
				glVertex3f(Radium * sin((j + 1) / 180.0*PI)*cos((i + 1) / 180.0*PI) * offset,
					Radium*cos((j + 1) / 180.0*PI) * offset,
					-Radium*sin((j + 1) / 180.0*PI)*sin((i + 1) / 180.0*PI) * offset
					);

				x_rate = (i + r + 1) % 360 / 360.0;
				y_rate = 1 - j / 180.0;
				X = x_rate*(bih.biWidth-1);
				Y = y_rate*(bih.biHeight-1);
				GetDIBColor(X, Y, &l_red, &l_green, &l_blue);
				R = l_red;
				G = l_green;
				B = l_blue;
				offset = CalculateOffset(R, G, B);
				glTexCoord2f((i + r + 1) / 360.0, 1 - j / 180.0);
				glVertex3f(Radium * sin(j / 180.0*PI)*cos((i + 1) / 180.0*PI) * offset,
					Radium*cos(j / 180.0*PI) * offset,
					-Radium*sin(j / 180.0*PI)*sin((i + 1) / 180.0*PI) * offset
					);
			}
		}
		glEnd();
		glPopMatrix();

		//glutTimerFunc(33, TimerFunction, 2);
		glDisable(GL_TEXTURE_2D);
	}

	void paint()
	{
		paintBackground();
		ReadRGB();
		paintGlobal();
		
	}
};