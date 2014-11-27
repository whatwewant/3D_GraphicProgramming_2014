#ifndef _IMAGE_LIB_
#define _IMAGE_LIB_

class CImage {
public:
	int width, height;
	int bpp;
	unsigned char *pixels;

	CImage (void) {width=0; height=0; bpp=1; pixels=NULL;}
	~CImage (void) {Clear();}
	void Clear (void) {
		if (pixels != NULL) {
			delete [] pixels;
			pixels = NULL;
		}
	}

	void FlipX(void);
	void FlipY(void);

	int Load (const char *file_name);

protected:
	int LoadJPEG (const char *file_name);
};

unsigned int Load2DTexture (const char *file_name);
unsigned int Load2DMipmapTexture (const char *file_name);

#endif