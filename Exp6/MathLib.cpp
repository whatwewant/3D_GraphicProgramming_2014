#include "MathLib.h"

void CQuaternion::GetRotationMatrix (CMatrix3D &R) {
	float m[4][4] = {
		1-2*y*y-2*z*z,	2*x*y-2*z*w,	2*x*z+2*y*w,	0,
		2*x*y+2*z*w,	1-2*x*x-2*z*z,	2*y*z-2*x*w,	0,
		2*x*z-2*y*w,	2*y*z+2*x*w,	1-2*x*x-2*y*y,	0,
		0,				0,				0,				1
	};
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++)
			R.m[i][j] = m[j][i];
	}
}

// OK
CQuaternion operator * (const CQuaternion &q1, const CQuaternion &q2) {
	CVector3D v1 = CVector3D(q1.x, q1.y, q1.z);
	CVector3D v2 = CVector3D(q2.x, q2.y, q2.z);

	CVector3D V = q1.w*v2 + q2.w*v1 + VectorCross(v1, v2);
	return CQuaternion (V.x, V.y, V.z, q1.w*q2.w - VectorDot(v1, v2));
	/*
	CQuaternion q;
	CVector3D v1(q1.x, q1.y, q1.z), v2(q2.x, q2.y, q2.z);
	q.w = q1.w*v2 + q2.w*v1 + VectorCross(v1, v2);

	return CQuaternion (V.x, V.y, V.z, q1.w*q2.w - VectorDot(v1, v2));
	*/
}