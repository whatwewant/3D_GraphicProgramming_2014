#include "MathLib.h"

#ifndef _CAMERA_
#define _CAMERA_

class CCamera
{
protected:
	float theta; // ˮƽ��ת��
	float phi;   // ��ֱ����
	CVector3D u0, v0, n0; // ��ʼʱ�����ң����Ϻ����λ����
	CVector3D v1, u1, n1;     // ʵʩˮƽ��ת�Ǻ����Һ����λ����
	
	void CalculateDirectionVectors(void);
public:
	CVector3D P0;         // �����λ��
	CVector3D u, v, n;    // ��ǰʱ�����ң����Ϻ����λ����

	void Init(const CVector3D& initial_pos, 
	const CVector3D& initial_up, const CVector3D& initial_back);

	void GetViewMatrix(CMatrix3D& view_mat);

	void MoveForward(float step);
	void MoveRight(float step);
	void MoveUp(float step);
	void TurnLeft(float angle);
	void LookUp(float angle);
};

#endif
