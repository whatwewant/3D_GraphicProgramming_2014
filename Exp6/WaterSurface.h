#ifndef _WATER_SURFACE_
#define _WATER_SURFACE_

#include "Mesh.h"

// ƽ�沨��
class CPlaneWave
{
public:
	float A;   // ���
	float f;   // ��Ƶ��
	float phi; // ��λ��
	float Lx_rcp;  // x���򲨳��ĵ���
	float Ly_rcp;  // y���򲨳��ĵ���

	CPlaneWave(void) 
	{ A=0.0f; f=0.0f; phi=0.0f; Lx_rcp=0.0f; Ly_rcp=0.0f; }
};

#define MAX_NUM_PLANE_WAVES 4  // ���ƽ�沨����

// ˮ����
class CWaterSurface: public CMesh
{
public:
	CPlaneWave plane_waves[MAX_NUM_PLANE_WAVES]; // ƽ�沨
	CVector3D *reflection_vectors; // ����ķ�������

	CWaterSurface(void) {reflection_vectors=NULL;}

	virtual void Release(void);
	virtual void Draw(void);

	void Create(float sx, float sy, int nx, int ny);
	void CalculateReflectionVectors(const CVector3D &camera_pos);
	void Animate(float time);
};

#endif
