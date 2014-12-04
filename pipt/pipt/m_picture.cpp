
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


/* 函数power_of_two
* 检查一个整数是否为2的整数次方，如果是，返回1，否则返回0
* 实际上只要查看其二进制位中有多少个，如果正好有1个，返回1，否则返回0
* 在“查看其二进制位中有多少个”时使用了一个小技巧
* 使用n &= (n-1)可以使得n中的减少一个（具体原理大家可以自己思考）
*/
int power_of_two(int n)
{
    if( n <= 0 )
        return 0;
    return (n & (n-1)) == 0;
}

/* 函数load_texture
* 读取一个BMP文件作为纹理
* 如果失败，返回0，如果成功，返回纹理编号
*/
GLuint load_texture(const char* file_name,bool useGaussianBlur)
{
     GLint width, height, total_bytes,last_texture_ID;
     GLubyte* pixels = 0;
     GLuint texture_ID = 0;

     // 打开文件，如果失败，返回
     FILE* pFile = fopen(file_name, "rb");
    if( pFile == 0 )
        return 0;

     // 读取文件中图象的宽度和高度
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);
    fread(&height, 4, 1, pFile);
    fseek(pFile, BMP_Header_Length, SEEK_SET);

     // 计算每行像素所占字节数，并根据此数据计算总像素字节数
     {
         GLint line_bytes = width * 3;
         while( line_bytes % 4 != 0 )
             ++line_bytes;
         total_bytes = line_bytes * height;
     }

     // 根据总像素字节数分配内存
     pixels = (GLubyte*)malloc(total_bytes);
    if( pixels == 0 )
     {
        fclose(pFile);
        return 0;
     }

     // 读取像素数据
    if( fread(pixels, total_bytes, 1, pFile) <= 0 )
     {
        free(pixels);
        fclose(pFile);
        return 0;
     }

     // 在旧版本的OpenGL中
     // 如果图象的宽度和高度不是的整数次方，则需要进行缩放
     // 这里并没有检查OpenGL版本，出于对版本兼容性的考虑，按旧版本处理
     // 另外，无论是旧版本还是新版本，
     // 当图象的宽度和高度超过当前OpenGL实现所支持的最大值时，也要进行缩放
     {
         GLint max;
         glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
        if( !power_of_two(width)
          || !power_of_two(height)
          || width > max
          || height > max )
         {
            const GLint new_width = 256;
            const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形
             GLint new_line_bytes, new_total_bytes;
             GLubyte* new_pixels = 0;

             // 计算每行需要的字节数和总字节数
             new_line_bytes = new_width * 3;
            while( new_line_bytes % 4 != 0 )
                 ++new_line_bytes;
             new_total_bytes = new_line_bytes * new_height;

             // 分配内存
             new_pixels = (GLubyte*)malloc(new_total_bytes);
            if( new_pixels == 0 )
             {
                free(pixels);
                fclose(pFile);
                return 0;
             }

             // 进行像素缩放
             gluScaleImage(GL_RGB,
                 width, height, GL_UNSIGNED_BYTE, pixels,
                 new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

             // 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
             free(pixels);
             pixels = new_pixels;
             width = new_width;
             height = new_height;
         }
     }
	 if(useGaussianBlur){
		pixels=Gaussian_Blur(pixels, height,width);
	 }
     // 分配一个新的纹理编号
     glGenTextures(1, &texture_ID);
    if( texture_ID == 0 )
     {
        free(pixels);
        fclose(pFile);
        return 0;
     }

     // 绑定新的纹理，载入纹理并设置纹理参数
     // 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
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

     // 之前为pixels分配的内存可在使用glTexImage2D以后释放
     // 因为此时像素数据已经被OpenGL另行保存了一份（可能被保存到专门的图形硬件中）
    free(pixels);
    return texture_ID;
}