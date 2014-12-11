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
unsigned int LoadCubemapTexture(
	const char *image_file_left, const char *image_file_right,
	const char *image_file_up, const char *image_file_down,
	const char *image_file_front, const char *image_file_back );
unsigned int Load2DTransparentTexture(const char *color_file_name, 
	const char *alpha_file_name);


#endif