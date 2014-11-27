#include "MathLib.h"

#ifndef __ARCBALL_H_
#define __ARCBALL_H_

class CArcBall {
protected:
	CVector3D start, end;
	float width, height;

	void MapToSphere (int x, int y, CVector3D &v) 
	{
		v.x = 2*x / width -1;
		v.y = -2*y / height + 1;
		v.z = (float)sqrt(1- v.x*v.x - v.y*v.y);
		float r = (float)sqrt(v.x*v.x + v.y*v.y);

		if (r > 1) {
			v.x = v.x/r;
			v.y = v.y/r;
			v.z = 0;
		}

		/*
		int xn = 2*x / width - 1;
		int yn = -2*y / height + 1;
		float r = (float)sqrt(xn*xn + yn*yn);
	
		v.x = xn;
		v.y = xn;
		v.z = (float)sqrt(1-xn*xn - yn*yn);

		if (r > 1)  {
			v.x = xn/r;
			v.y = yn/r;
			v.z = 0.0;
		}*/
	
	}

public:
	void SetSize (int width0, int height0)
	{
		width = (float)width0;
		height = (float)height0;
	}

	void SetStartPoint (int x, int y) {
		MapToSphere(x, y, start);
	}

	void SetEndPoint (int x, int y) {
		MapToSphere(x, y, end);
	}

	void GetQuaternion (CQuaternion &quat)
	{
		CVector3D v = VectorCross(start, end);
		quat.w = VectorDot(start, end);
		quat.x = v.x;
		quat.y = v.y;
		quat.z = v.z;
	}
};

#endif