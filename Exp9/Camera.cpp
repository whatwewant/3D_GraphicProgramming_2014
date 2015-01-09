#include "Camera.h"

void CCamera::CalculateDirectionVectors(void)
{
	float c=cos((float)theta);
	float s=sin((float)theta);

	u1=c*u0-s*n0;
	n1=s*u0+c*n0;

	c=cos((float)phi);
	s=sin((float)phi);

	u=u1;
	v=c*v0+s*n1;
	n=-s*v0+c*n1;
}

void CCamera::Init(const CVector3D& initial_pos, 
const CVector3D& initial_up, const CVector3D& initial_back)
{
	P0=initial_pos;
	v0=initial_up;
	n0=initial_back;
	u0=VectorCross(v0, n0);
	theta=0.0f;
	phi=0.0f;
	CalculateDirectionVectors();
}

void CCamera::GetViewMatrix(CMatrix3D& view_mat)
{
	view_mat.m[0][0]=u.x;
	view_mat.m[1][0]=u.y;
	view_mat.m[2][0]=u.z;
	view_mat.m[3][0]=-VectorDot(u,P0);
	view_mat.m[0][1]=v.x;
	view_mat.m[1][1]=v.y;
	view_mat.m[2][1]=v.z;
	view_mat.m[3][1]=-VectorDot(v,P0);
	view_mat.m[0][2]=n.x;
	view_mat.m[1][2]=n.y;
	view_mat.m[2][2]=n.z;
	view_mat.m[3][2]=-VectorDot(n,P0);
	view_mat.m[0][3]=0;
	view_mat.m[1][3]=0;
	view_mat.m[2][3]=0;
	view_mat.m[3][3]=1;
}

void CCamera::MoveForward(float step)
{
	P0-=step*n1;
}

void CCamera::MoveRight(float step)
{
	P0+=step*u1;
}

void CCamera::MoveUp(float step)
{
	P0+=step*v0;
}

void CCamera::TurnLeft(float angle)
{
	theta+=(float)(angle*PI_OVER_180);
	CalculateDirectionVectors();
}

void CCamera::LookUp(float angle)
{
	phi+=(float)(angle*PI_OVER_180);
	CalculateDirectionVectors();
}
