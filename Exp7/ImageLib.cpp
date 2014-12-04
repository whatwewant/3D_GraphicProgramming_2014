#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <string.h>
#include "GL/glext.h"

#include "ImageLib.h"
#include "jpeglib.h"
#include "jerror.h"
#pragma comment(lib, "jpeg.lib")

void CImage::FlipX (void)
{
	int rowsize = width * bpp;
	unsigned char temp_buffer[4];
	unsigned char * buffer = pixels;

	for (int i=0; i<height; ++i)
	{
		for (int j=0; j<width/2; ++j)
		{
			memcpy (temp_buffer, buffer+j*bpp, bpp);
			memcpy (buffer+j*bpp, buffer+(width-1-j)*bpp, bpp);
			memcpy ( buffer+(width-1-j)*bpp, temp_buffer, bpp);
		}
	}
}

void CImage::FlipY(void)
{
	int rowsize=width*bpp;
	unsigned char *temp_buffer=new unsigned char[rowsize];
	for (int i=0; i<height/2; ++i)
	{
		unsigned char *prow1=pixels+i*rowsize;
		unsigned char *prow2=pixels+(height-1-i)*rowsize;
		memcpy(temp_buffer, prow1, rowsize);
		memcpy(prow1, prow2, rowsize);
		memcpy(prow2, temp_buffer, rowsize);
	}
	delete [] temp_buffer;
}

int CImage::LoadJPEG(const char *file_name)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	
	FILE *fp= fopen(file_name, "rb");
	if (fp==NULL) return 0;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	width=cinfo.output_width;
	height=cinfo.output_height;
	bpp=cinfo.output_components;
	int rowsize=width*bpp;

	Clear();
	pixels=new unsigned char [rowsize*height];
	unsigned char *buffer=pixels;

	while (cinfo.output_scanline < cinfo.output_height) 
	{
		jpeg_read_scanlines(&cinfo, &buffer, 1);
		buffer+=rowsize;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(fp);

	FlipY();

	return 1;
}

int CImage::Load(const char *file_name)
{
	char *str=strrchr(file_name, '.');
	if (str==NULL) return 0;
	++str;
	if (stricmp(str, "jpg")==0 || stricmp(str, "jpeg")==0 || stricmp(str, "jpe")==0)
		return LoadJPEG(file_name);
	else return 0;
}

unsigned int Load2DTexture(const char *file_name)
{
	CImage img;
	if (!img.Load(file_name)) return 0;

	static GLenum pixel_formats[4]={GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA};

	unsigned int itex;
	glGenTextures(1, &itex);

	glBindTexture(GL_TEXTURE_2D, itex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, pixel_formats[img.bpp-1], 
	img.width, img.height, 0,
	pixel_formats[img.bpp-1], GL_UNSIGNED_BYTE, img.pixels);

	return itex;
}

unsigned int Load2DMipmapTexture(const char *file_name)
{
	CImage img;
	if (!img.Load(file_name)) return 0;

	static GLenum pixel_formats[4]={GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA};

	unsigned int itex;
	glGenTextures(1, &itex);

	glBindTexture(GL_TEXTURE_2D, itex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, pixel_formats[img.bpp-1], img.width, img.height,
		pixel_formats[img.bpp-1], GL_UNSIGNED_BYTE, img.pixels);

	return itex;
}

unsigned int LoadCubemapTexture(
	const char *image_file_left, const char *image_file_right,
	const char *image_file_up, const char *image_file_down,
	const char *image_file_front, const char *image_file_back )
{
	unsigned int itex;
	glGenTextures(1, &itex);

	glBindTexture(GL_TEXTURE_CUBE_MAP, itex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	static GLenum cubemap_target[6]={
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	static const char *cubemap_image[6]={
		image_file_left, image_file_right,
		image_file_up, image_file_down,
		image_file_front, image_file_back
	};

	static GLenum pixel_formats[4]={GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA};

	CImage img;
	for (int i=0; i<6; ++i)
	{
		if(!img.Load(cubemap_image[i]))
		{
			glDeleteTextures(1, &itex);
			return 0;
		}
		
		img.FlipX();
		img.FlipY();

		glTexImage2D(cubemap_target[i], 0, pixel_formats[img.bpp-1], 
			img.width, img.height, 0, 
			pixel_formats[img.bpp-1], GL_UNSIGNED_BYTE, img.pixels);
	}

	return itex;
}

unsigned int Load2DTransparentTexture(const char *color_file_name, 
	const char *alpha_file_name)
{
	// 载入颜色图像和alpha图像
	CImage color_img, alpha_img;
	if (!color_img.Load(color_file_name)) return 0;
	if (!alpha_img.Load(alpha_file_name)) return 0;

	// 检查两幅图像大小是否一样
	if (color_img.width!=alpha_img.width) return 0;
	if (color_img.height!=alpha_img.height) return 0;
	if (color_img.bpp!=3) return 0;
	if (alpha_img.bpp!=1) return 0;

	// 将两幅图像组合成一幅图像
	int w=color_img.width;
	int h=color_img.height;
	int npixels=w*h;
	unsigned char *buffer=new unsigned char [npixels*4];
	for (int i=0; i<npixels; ++i)
	{
		buffer[i*4]=color_img.pixels[i*3];
		buffer[i*4+1]=color_img.pixels[i*3+1];
		buffer[i*4+2]=color_img.pixels[i*3+2];
		buffer[i*4+3]=alpha_img.pixels[i];
	}

	// 将图像载入纹理
	unsigned int itex;
	glGenTextures(1, &itex);

	glBindTexture(GL_TEXTURE_2D, itex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	delete [] buffer;
	return itex;
}
