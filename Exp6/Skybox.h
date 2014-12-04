#ifndef _SKYBOX_
#define _SKYBOX_

#include "MathLib.h"

// ��պ���
class CSkybox
{
public:
	float size;  // ��С

	CVector3D vertices[8];     // ��������
	unsigned char indices[36]; // �����ζ�������

	CSkybox(void) {size=10.0f;}
	void Create(void);
	void Draw(void);
};

#endif
