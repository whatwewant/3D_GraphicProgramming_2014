#ifndef _WATER_SURFACE_
#define _WATER_SURFACE_

#include "Mesh.h"

// 平面波类
class CPlaneWave
{
public:
	float A;   // 振幅
	float f;   // 振荡频率
	float phi; // 相位角
	float Lx_rcp;  // x方向波长的倒数
	float Ly_rcp;  // y方向波长的倒数

	CPlaneWave(void) 
	{ A=0.0f; f=0.0f; phi=0.0f; Lx_rcp=0.0f; Ly_rcp=0.0f; }
};

#define MAX_NUM_PLANE_WAVES 4  // 最多平面波数量

// 水面类
class CWaterSurface: public CMesh
{
public:
	CPlaneWave plane_waves[MAX_NUM_PLANE_WAVES]; // 平面波
	CVector3D *reflection_vectors; // 顶点的反射向量

	CWaterSurface(void) {reflection_vectors=NULL;}

	virtual void Release(void);
	virtual void Draw(void);

	void Create(float sx, float sy, int nx, int ny);
	void CalculateReflectionVectors(const CVector3D &camera_pos);
	void Animate(float time);
};

#endif
