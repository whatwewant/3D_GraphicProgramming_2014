#include"MathLib.h"

void  CQuaternion::GetRotationMatrix(CMatrix3D &R)
{
	R.m[0][0]=1-2*y*y-2*z*z; R.m[1][0]=2*x*y-2*w*z;   R.m[2][0]=2*x*z+2*w*y;   R.m[3][0]=0;
	R.m[0][1]=2*x*y+2*w*z;   R.m[1][1]=1-2*x*x-2*z*z; R.m[2][1]=2*y*z-2*w*x;   R.m[3][1]=0;
	R.m[0][2]=2*x*z-2*w*y;   R.m[1][2]=2*y*z+2*w*x;   R.m[2][2]=1-2*x*x-2*y*y; R.m[3][2]=0;
	R.m[0][3]=0;             R.m[1][3]=0;             R.m[2][3]=0;             R.m[3][3]=1;
}

CQuaternion operator * (const CQuaternion &q1,const CQuaternion &q2)
{
	CVector3D v1,v2,v;
	v1.x=q1.x;v1.y=q1.y;v1.z=q1.z;
	v2.x=q2.x;v2.y=q2.y;v2.z=q2.z;
    v=q1.w*v2+q2.w*v1+VectorCross(v1,v2);
	return CQuaternion(v.x,v.y,v.z,(q1.w*q2.w-VectorDot(v1,v2)));
}

void CQuaternion::GetInverse(CQuaternion &qinv)
{
	CVector3D a;
	float q2;
	a.x=x;
	a.y=y;
	a.z=z;
	q2=w*w+VectorDot(a,a);
	qinv.w=1/q2*w;
	qinv.x=1/q2*(-a.x);
	qinv.y=1/q2*(-a.y);
	qinv.z=1/q2*(-a.z);
}

CVector3D CQuaternion::TransformPoint(const CVector3D &p)
{
	CQuaternion p1,p2,q,q1;
	p1.w=0.0f;p1.x=p.x;p1.y=p.y;p1.z=p.z;
	q.x=x;q.y=y;q.z=z;q.w=w;
    GetInverse(q1);
	p2=q*p1*q1;
	return CVector3D(p2.x,p2.y,p2.z);
}

CQuaternion QuaternionSlerp(const CQuaternion &p1, const CQuaternion &p2, float u)
{
	CQuaternion p;
	double ou,Cou;
	Cou=p1.w*p2.w+p1.x*p2.x+p1.y*p2.y+p1.z*p2.z;
	ou=acos(Cou);
	if(Cou>=0)
	{
		if(1-Cou<0.005)
		{
			CQuaternion p3;
			p3=p2-p1;
			p3.w*=u;p3.x*=u;p3.y*=u;p3.z*=u;
			p=p3+p1;
			return p;
		}
		else
		{
			p.w=sin((1-u)*ou)/sin(ou)*p1.w+sin(u*ou)/sin(ou)*p2.w;
			p.x=sin((1-u)*ou)/sin(ou)*p1.x+sin(u*ou)/sin(ou)*p2.x;
			p.y=sin((1-u)*ou)/sin(ou)*p1.y+sin(u*ou)/sin(ou)*p2.y;
			p.z=sin((1-u)*ou)/sin(ou)*p1.z+sin(u*ou)/sin(ou)*p2.z;
			return p;
		}
	}
	else
	{
		if(Cou-1<0.005)
		{
			CQuaternion p4;
			p4=-p2-p1;
			p4.w*=u;p4.x*=u;p4.y*=u;p4.z*=u;
			p=p1+p4;
			return p;
		}
		else
		{
			p.w=sin((1-u)*(PI-ou))/sin(PI-ou)*p1.w-sin(u*(PI-ou))/sin(PI-ou)*p2.w;
			p.x=sin((1-u)*(PI-ou))/sin(PI-ou)*p1.x-sin(u*(PI-ou))/sin(PI-ou)*p2.x;
			p.y=sin((1-u)*(PI-ou))/sin(PI-ou)*p1.y-sin(u*(PI-ou))/sin(PI-ou)*p2.y;
			p.z=sin((1-u)*(PI-ou))/sin(PI-ou)*p1.z-sin(u*(PI-ou))/sin(PI-ou)*p2.z;
			return p;
		}
	}
}