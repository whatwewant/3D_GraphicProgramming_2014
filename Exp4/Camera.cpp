#include "Camera.h"

void CCamera::CalculateDirectionVectors(void)
{
	u1 = u0*cos(theta) - n0*sin(theta);
	v1 = v0;
	n1 = u0*sin(theta) + n0*cos(theta);

	u = u1;
	v = v1*cos(phi) + n1*sin(phi);
	n = -v1*sin(phi) + n1*cos(phi);
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
	float tmp[4][4] = {
		u.x, u.y, u.z, -VectorDot(u, P0),
		v.x, v.y, v.z, -VectorDot(v, P0),
		n.x, n.y, n.z, -VectorDot(n, P0),
		0, 0, 0, 1
	};

	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			view_mat.m[i][j] = tmp[j][i];
		}
	}
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
