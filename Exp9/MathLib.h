#include <math.h>
#include <string.h>

#ifndef _MATH_LIB_
#define _MATH_LIB_

#define PI 3.14159265359
#define PI_OVER_180 0.01745329252
#define PI_UNDER_180 57.29577951308

class CVector3D
{
public:
	float x,y,z;
	CVector3D(void){x=0.0;y=0.0;z=0.0;}
	CVector3D(float x0,float y0,float z0)
	{
		x=x0;y=y0;z=z0;
	}
	void set(float x0,float y0,float z0)
	{
		x=x0;y=y0;z=z0;
	}

	float Length(void)
	{
		return (float)sqrt(x*x+y*y+z*z);
	}

	void Normalize(void)
	{
		float s=(float)(1.0/sqrt(x*x+y*y+z*z));
		x*=s;y*=s;z*=s;
	}
};

//单边向量操作
inline CVector3D operator - (const CVector3D& v)
{return CVector3D(-v.x,-v.y,-v.z);}

//向量-向量操作
inline CVector3D operator + (const CVector3D& v1,const CVector3D& v2)
{return CVector3D(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);}

inline CVector3D operator - (const CVector3D& v1,const CVector3D& v2)
{return CVector3D(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);}

inline void operator += (CVector3D& v1,const CVector3D& v2)
{v1.x+=v2.x;v1.y+=v2.y;v1.z+=v2.z;}

inline void operator -= (CVector3D& v1,const CVector3D& v2)
{v1.x-=v2.x;v1.y-=v2.y;v1.z-=v2.z;}

//向量-标量操作
inline CVector3D operator + (const CVector3D& v,float c)
{return CVector3D(v.x+c,v.y+c,v.z+c);}

inline CVector3D operator - (const CVector3D& v,float c)
{return CVector3D(v.x-c,v.y-c,v.z-c);}

inline CVector3D operator * (const CVector3D& v,float c)
{return CVector3D(v.x*c,v.y*c,v.z*c);}

inline CVector3D operator / (const CVector3D& v,float c)
{return CVector3D(v.x/c,v.y/c,v.z/c);}

inline void operator += (CVector3D& v,float c)
{v.x+=c;v.y+=c;v.z+=c;}

inline void operator -= (CVector3D& v,float c)
{v.x-=c;v.y-=c;v.z-=c;}

inline void operator *= (CVector3D& v,float c)
{v.x*=c;v.y*=c;v.z*=c;}

inline void operator /= (CVector3D& v,float c)
{v.x/=c;v.y/=c;v.z/=c;}

//标量-向量操作
inline CVector3D operator + (float c,const CVector3D& v)
{return CVector3D(c+v.x,c+v.y,c+v.z);}

inline CVector3D operator - (float c,const CVector3D& v)
{return CVector3D(c-v.x,c-v.y,c-v.z);}

inline CVector3D operator * (float c,const CVector3D& v)
{return CVector3D(c*v.x,c*v.y,c*v.z);}

//向量点乘和叉乘
inline float VectorDot(const CVector3D& v1,const CVector3D& v2)
{return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;}

inline CVector3D VectorCross(const CVector3D& v1,const CVector3D& v2)
{return CVector3D(v1.y*v2.z-v1.z*v2.y,v1.z*v2.x-v1.x*v2.z,v1.x*v2.y-v1.y*v2.x);}

inline CVector3D TriangleNormal(const CVector3D &v0, const CVector3D &v1, const CVector3D &v2)
{
	CVector3D n=VectorCross(v1-v0, v2-v0);
	n.Normalize();
	return n;
}


class CMatrix3D
{
public:
	float m[4][4];
};

class CQuaternion
{
public:
	float x,y,z,w;
	CQuaternion(void){x=0.0;y=0.0;z=0.0;w=1.0;}
	CQuaternion(float x0,float y0,float z0,float w0)
	{
		x=x0;y=y0;z=z0;w=w0;
	}
	void SetIdentity(void)
	{
		x=0.0;y=0.0;z=0.0;w=1.0;
	}
	void GetRotationMatrix(CMatrix3D &R);
	void CalculateScalar(void)
	{
		double t=1.0-x*x-y*y-z*z;
		if (t<=0.0) w=0.0f; else w=(float)sqrt(t);
	}
	void GetInverse(CQuaternion &qinv);
	CVector3D TransformPoint(const CVector3D &p);
};

inline CQuaternion operator - (const CQuaternion& q)
{return CQuaternion(-q.x,-q.y,-q.z,-q.w);}

inline CQuaternion operator + (const CQuaternion& q1,const CQuaternion& q2)
{return CQuaternion(q1.x+q2.x,q1.y+q2.y,q1.z+q2.z,q1.w+q2.w);}

inline CQuaternion operator - (const CQuaternion& q1,const CQuaternion& q2)
{return CQuaternion(q1.x-q2.x,q1.y-q2.y,q1.z-q2.z,q1.w-q2.w);}

CQuaternion operator * (const CQuaternion& q1,const CQuaternion& q2);

CQuaternion QuaternionSlerp(const CQuaternion &p1, const CQuaternion &p2, float u);

class CBoundingBox
{
public:
	CVector3D min, max;
	CVector3D GetCenter(void) { return 0.5f*(min+max); }
	CVector3D GetSize(void) { return max-min; }
	void operator +=(const CBoundingBox& bbox1)
	{
		if (min.x>bbox1.min.x) min.x=bbox1.min.x;
		if (min.y>bbox1.min.y) min.y=bbox1.min.y;
		if (min.z>bbox1.min.z) min.z=bbox1.min.z;
		if (max.x<bbox1.max.x) max.x=bbox1.max.x;
		if (max.y<bbox1.max.y) max.y=bbox1.max.y;
		if (max.z<bbox1.max.z) max.z=bbox1.max.z;
	}
};

#endif