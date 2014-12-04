
#include <math.h>
#include "m_picture.h"

#define BMP_Header_Length 54
#define PI 3.1415927
#define LENGTH 5
#define SIGMA 3
GLuint load_texture(const char* file_name);
float* Gaussian_Blur_Init(int length,float sigma);
float Gaussian_Blur_getWeight(int x,int y);

void MPicture::paint(){
			
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(7.0f, 7.0f, -0.5f);
		
		glEnable(GL_TEXTURE_2D);
		GLuint texGround = load_texture("cap18.bmp",true);
		glBindTexture(GL_TEXTURE_2D, texGround);
		glBegin(GL_POLYGON);
		glColor3f(0,0,0);
		glTexCoord2f(1.0f, 1.0f);glVertex3f(P_WIDTH,P_WIDTH,0);
		glTexCoord2f(0.0f, 1.0f);glVertex3f(0,P_WIDTH,0);
		glTexCoord2f(0.0f, 0.0f);glVertex3f(0,0,0);
		glTexCoord2f(1.0f, 0.0f);glVertex3f(P_WIDTH,0,0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		
		glEnable(GL_TEXTURE_2D);
		GLuint texGround2 = load_texture("cap18.bmp",false);
		glBindTexture(GL_TEXTURE_2D, texGround2);
		glBegin(GL_POLYGON);
		glColor3f(0,0,0);
		glTexCoord2f(1.0f, 1.0f);glVertex3f(P_WIDTH+P_WIDTH,P_WIDTH,0);
		glTexCoord2f(0.0f, 1.0f);glVertex3f(P_WIDTH+0,P_WIDTH,0);
		glTexCoord2f(0.0f, 0.0f);glVertex3f(P_WIDTH+0,0,0);
		glTexCoord2f(1.0f, 0.0f);glVertex3f(P_WIDTH+P_WIDTH,0,0);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
		
}

int Gaussian_Blur_Kernel_Length=1;

GLubyte* Gaussian_Blur(GLubyte* pixels,int height, int width)
{
	int pixel_size=3;
	int total_size=height*width*pixel_size;
	GLubyte* new_pixels=(GLubyte*)malloc(total_size);
	float* kernel=Gaussian_Blur_Init(LENGTH,SIGMA);
	//Gaussian Blur
	for(int i=0;i<total_size;i++){
		if(i==6000){
			i=6000;
		}
		float temp=0;
		for(int offset_x=-Gaussian_Blur_Kernel_Length+1;offset_x<=Gaussian_Blur_Kernel_Length-1;offset_x++){
			for(int offset_y=-Gaussian_Blur_Kernel_Length+1;offset_y<=Gaussian_Blur_Kernel_Length-1;offset_y++){
				int pos=i+pixel_size*offset_x+pixel_size*width*offset_y;
				if(pos>=0&&pos<total_size){
					temp+=pixels[i+pixel_size*offset_x+pixel_size*width*offset_y]*kernel[offset_x+Gaussian_Blur_Kernel_Length-1+(Gaussian_Blur_Kernel_Length*2-1)*(offset_y+Gaussian_Blur_Kernel_Length-1)];
				}
			}
		}
		if(temp>255){
			temp=255;
		}
		new_pixels[i]=temp;
	}
	
	return new_pixels;
}

float Gaussian_Blur_Kernel_Sigma;
//given length, return the kernel
float* Gaussian_Blur_Init(int length,float sigma){
	float* Gaussian_Blur_Kernel;
	Gaussian_Blur_Kernel=(float*)malloc((length*2-1)*(length*2-1)*sizeof(float));
	Gaussian_Blur_Kernel_Length=length;
	Gaussian_Blur_Kernel_Sigma=sigma;
	float sum=0;
	for(int offset_x=-Gaussian_Blur_Kernel_Length+1;offset_x<=Gaussian_Blur_Kernel_Length-1;offset_x++){
			for(int offset_y=-Gaussian_Blur_Kernel_Length+1;offset_y<=Gaussian_Blur_Kernel_Length-1;offset_y++){
				
				Gaussian_Blur_Kernel[offset_x+Gaussian_Blur_Kernel_Length-1+(length*2-1)*(offset_y+Gaussian_Blur_Kernel_Length-1)]=1.0/2/PI/sigma/sigma*exp(-1.0*(offset_x*offset_x+offset_y*offset_y)/2/sigma/sigma);
				sum+=Gaussian_Blur_Kernel[offset_x+Gaussian_Blur_Kernel_Length-1+(length*2-1)*(offset_y+Gaussian_Blur_Kernel_Length-1)];
			}
	}
	//sum=sum;
	for(int offset_x=-Gaussian_Blur_Kernel_Length+1;offset_x<=Gaussian_Blur_Kernel_Length-1;offset_x++){
			for(int offset_y=-Gaussian_Blur_Kernel_Length+1;offset_y<=Gaussian_Blur_Kernel_Length-1;offset_y++){
				
				Gaussian_Blur_Kernel[offset_x+Gaussian_Blur_Kernel_Length-1+(length*2-1)*(offset_y+Gaussian_Blur_Kernel_Length-1)]=Gaussian_Blur_Kernel[offset_x+Gaussian_Blur_Kernel_Length-1+(length*2-1)*(offset_y+Gaussian_Blur_Kernel_Length-1)]/sum;
				
			}
	}
	return  Gaussian_Blur_Kernel;
}


/* ����power_of_two
* ���һ�������Ƿ�Ϊ2�������η�������ǣ�����1�����򷵻�0
* ʵ����ֻҪ�鿴�������λ���ж��ٸ������������1��������1�����򷵻�0
* �ڡ��鿴�������λ���ж��ٸ���ʱʹ����һ��С����
* ʹ��n &= (n-1)����ʹ��n�еļ���һ��������ԭ���ҿ����Լ�˼����
*/
int power_of_two(int n)
{
    if( n <= 0 )
        return 0;
    return (n & (n-1)) == 0;
}

/* ����load_texture
* ��ȡһ��BMP�ļ���Ϊ����
* ���ʧ�ܣ�����0������ɹ�������������
*/
GLuint load_texture(const char* file_name,bool useGaussianBlur)
{
     GLint width, height, total_bytes,last_texture_ID;
     GLubyte* pixels = 0;
     GLuint texture_ID = 0;

     // ���ļ������ʧ�ܣ�����
     FILE* pFile = fopen(file_name, "rb");
    if( pFile == 0 )
        return 0;

     // ��ȡ�ļ���ͼ��Ŀ�Ⱥ͸߶�
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);
    fread(&height, 4, 1, pFile);
    fseek(pFile, BMP_Header_Length, SEEK_SET);

     // ����ÿ��������ռ�ֽ����������ݴ����ݼ����������ֽ���
     {
         GLint line_bytes = width * 3;
         while( line_bytes % 4 != 0 )
             ++line_bytes;
         total_bytes = line_bytes * height;
     }

     // �����������ֽ��������ڴ�
     pixels = (GLubyte*)malloc(total_bytes);
    if( pixels == 0 )
     {
        fclose(pFile);
        return 0;
     }

     // ��ȡ��������
    if( fread(pixels, total_bytes, 1, pFile) <= 0 )
     {
        free(pixels);
        fclose(pFile);
        return 0;
     }

     // �ھɰ汾��OpenGL��
     // ���ͼ��Ŀ�Ⱥ͸߶Ȳ��ǵ������η�������Ҫ��������
     // ���ﲢû�м��OpenGL�汾�����ڶ԰汾�����ԵĿ��ǣ����ɰ汾����
     // ���⣬�����Ǿɰ汾�����°汾��
     // ��ͼ��Ŀ�Ⱥ͸߶ȳ�����ǰOpenGLʵ����֧�ֵ����ֵʱ��ҲҪ��������
     {
         GLint max;
         glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
        if( !power_of_two(width)
          || !power_of_two(height)
          || width > max
          || height > max )
         {
            const GLint new_width = 256;
            const GLint new_height = 256; // �涨���ź��µĴ�СΪ�߳���������
             GLint new_line_bytes, new_total_bytes;
             GLubyte* new_pixels = 0;

             // ����ÿ����Ҫ���ֽ��������ֽ���
             new_line_bytes = new_width * 3;
            while( new_line_bytes % 4 != 0 )
                 ++new_line_bytes;
             new_total_bytes = new_line_bytes * new_height;

             // �����ڴ�
             new_pixels = (GLubyte*)malloc(new_total_bytes);
            if( new_pixels == 0 )
             {
                free(pixels);
                fclose(pFile);
                return 0;
             }

             // ������������
             gluScaleImage(GL_RGB,
                 width, height, GL_UNSIGNED_BYTE, pixels,
                 new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

             // �ͷ�ԭ�����������ݣ���pixelsָ���µ��������ݣ�����������width��height
             free(pixels);
             pixels = new_pixels;
             width = new_width;
             height = new_height;
         }
     }
	 if(useGaussianBlur){
		pixels=Gaussian_Blur(pixels, height,width);
	 }
     // ����һ���µ�������
     glGenTextures(1, &texture_ID);
    if( texture_ID == 0 )
     {
        free(pixels);
        fclose(pFile);
        return 0;
     }

     // ���µ������������������������
     // �ڰ�ǰ���Ȼ��ԭ���󶨵������ţ��Ա��������лָ�
     glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
     glBindTexture(GL_TEXTURE_2D, texture_ID);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
         GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
     glBindTexture(GL_TEXTURE_2D, last_texture_ID);

     // ֮ǰΪpixels������ڴ����ʹ��glTexImage2D�Ժ��ͷ�
     // ��Ϊ��ʱ���������Ѿ���OpenGL���б�����һ�ݣ����ܱ����浽ר�ŵ�ͼ��Ӳ���У�
    free(pixels);
    return texture_ID;
}