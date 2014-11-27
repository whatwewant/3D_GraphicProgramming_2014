#include<math.h>
#include<string.h>

#ifndef _MATH_LIB_
#define _MATH_LIB_

#define PI				3.14159265359
#define PI_OVER_180		0.01745329252 // PI/180
#define PI_UNDER_180	57.29577951308 // 180/PI

class CVector3D {
public:
	float x, y, z;

	CVector3D(void) {
		x = 0.0;
		y = x;
		z = y;
	}
	CVector3D(float x0, float y0, float z0) {
		x = x0;
		y = y0;
		z = z0;
	}
	void Set (float x0, float y0, float z0) {
		x = x0;
		y = y0;
		z = z0;
	}

	float Length(void) {
		return (float)sqrt(x*x + y*y + z*z);
	}

	void Normalize(void) {
		float s = (float)(1.0 / sqrt(x*x + y*y + z*z));
		x *= s;
		y *= s;
		z *= s;
	}

};


/////
// Single Vector
	inline CVector3D operator - (const CVector3D &v) {
		return CVector3D(-v.x, -v.y, -v.z);
	}

	// Vector - Vector
	inline CVector3D operator + (const CVector3D &v1, const CVector3D &v2) {
		return CVector3D (v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
	}

	inline CVector3D operator - (const CVector3D &v1, const CVector3D &v2) {
		return CVector3D (v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
	}

	inline void operator += (CVector3D &v1, const CVector3D &v2) {
		v1.x += v2.x;
		v1.y += v2.y;
		v1.z += v2.z;
	}

	inline void operator -= (CVector3D &v1, const CVector3D &v2) {
		v1.x -= v2.x;
		v1.y -= v2.y;
		v1.z -= v2.z;
	}

	// Vertor - 
	inline CVector3D operator + (const CVector3D &v, float c) {
		return CVector3D (v.x+c, v.y+c, v.z+c);
	}

	inline CVector3D operator - (const CVector3D & v, float c) {
		return CVector3D (v.x-c, v.y-c, v.z-c);
	}

	inline CVector3D operator * (const CVector3D &v, float c) {
		return CVector3D (v.x*c, v.y*c, v.z*c);
	}

	inline CVector3D operator / (const CVector3D &v, float c) {
		return CVector3D (v.x/c, v.y/c, v.z/c);
	}

	inline void operator += (CVector3D &v, float c) {
		v.x += c;
		v.y += c;
		v.z += c;
	}

	inline void operator -= (CVector3D &v, float c) {
		v.x -= c;
		v.y -= c;
		v.z -= c;
	}

	inline void operator *= (CVector3D &v, float c) {
		v.x *= c;
		v.y *= c;
		v.z *= c;
	}
	
	inline void operator /= (CVector3D &v, float c) {
		v.x /= c;
		v.y /= c;
		v.z /= c;
	}

	// - Vector
	inline CVector3D operator + (float c, const CVector3D &v) {
		return CVector3D (c+v.x, c+v.y, c+v.z);
	}

	inline CVector3D operator - (float c, const CVector3D &v) {
		return CVector3D (c-v.x, c-v.y, c-v.z);
	}

	inline CVector3D operator * (float c, const CVector3D &v) {
		return CVector3D (c*v.x, c*v.y, c*v.z);
	}

	// Vector * Vertor Vector x Vector
	inline float VectorDot (const CVector3D &v1, const CVector3D &v2) {
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}

	inline CVector3D VectorCross (const CVector3D &v1, const CVector3D &v2) {
		return CVector3D (
			v1.y*v2.z - v1.z*v2.y,
			v1.z*v2.x - v1.x*v2.z,
			v1.x*v2.y - v1.y*v2.x
			);
	}
///



class CMatrix3D {
public:
	float m[4][4];
};


class CQuaternion {
public:
	float x, y, z, w;

	CQuaternion(void) {
		x = 0.0;
		y = 0.0;
		z = 0.0;
		w = 1.0;
	}

	CQuaternion(float x0, float y0, float z0, float w0) {
		x = x0;
		y = y0;
		z = z0;
		w = w0;
	}

	void SetIdentity (void) {
		x = 0.0;
		y = 0.0;
		z = 0.0;
		w = 1.0;
	}
	
	void GetRotationMatrix (CMatrix3D &R);
	
};

	// Single
	inline CQuaternion operator - (const CQuaternion &q) {
		return CQuaternion (-q.x, -q.y, -q.z, -q.w);
	}

	// CQuaternion - CQuaternion
	inline CQuaternion operator + (const CQuaternion &q1, const CQuaternion &q2) {
		return CQuaternion (q1.x+q2.x, q1.y+q2.y, q1.z+q2.z, q1.w+q2.w);
	}

	inline CQuaternion operator - (const CQuaternion &q1, const CQuaternion &q2) {
		return CQuaternion (q1.x-q2.x, q1.y-q2.y, q1.z-q2.z, q1.w-q2.w);
	}
	CQuaternion operator * (const CQuaternion &q1, const CQuaternion &q2);



#endif