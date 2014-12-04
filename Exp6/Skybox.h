#ifndef _SKYBOX_
#define _SKYBOX_

#include "MathLib.h"

// 天空盒类
class CSkybox
{
public:
	float size;  // 大小

	CVector3D vertices[8];     // 顶点坐标
	unsigned char indices[36]; // 三角形顶点索引

	CSkybox(void) {size=10.0f;}
	void Create(void);
	void Draw(void);
};

#endif
