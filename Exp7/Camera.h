#include "MathLib.h"

#ifndef _CAMERA_
#define _CAMERA_

class CCamera
{
protected:
	float theta; // 水平旋转角
	float phi;   // 垂直仰角
	CVector3D u0, v0, n0; // 初始时刻向右，向上和向后单位向量
	CVector3D v1, u1, n1;     // 实施水平旋转角后向右和向后单位向量
	
	void CalculateDirectionVectors(void);
public:
	CVector3D P0;         // 照相机位置
	CVector3D u, v, n;    // 当前时刻向右，向上和向后单位向量

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
