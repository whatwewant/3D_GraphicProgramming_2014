#ifndef _CLOTH_
#define _CLOTH_

#include "Mesh.h"

// 布料粒子类
class CClothParticle
{
public:
	float mass;  // 质量
	CVector3D v; // 速度
	CVector3D F; // 力
	unsigned char fixed; // 固定标志
};

// 布料弹簧类
class CClothSpring
{
public:
	float length0; // 原始长度
	unsigned long v0, v1; // 弹簧连接的两个粒子的索引
};

// 布料类
class CCloth: public CMesh
{
protected:
	void CalculateSpringInitialLengths
	(unsigned long num_springs, CClothSpring *springs);
	// num_springs: 弹簧数量
	// springs: 弹簧数组

	void _ApplySpringForces(float k_spring, float c_spring,
	 unsigned long num_springs, const CClothSpring *springs);
	// k_spring: 弹簧弹性系数
	// c_spring: 弹簧阻尼系数
	// num_springs: 弹簧数量
	// springs: 弹簧数组

public:
	float thickness; // 厚度
	float density;   // 密度(单位面积上的质量)
	
	float k_tensile; // 拉伸弹簧弹性系数
	float k_shear;   // 剪切弹簧弹性系数
	float k_bending; // 弯曲弹簧弹性系数
	
	float c_tensile; // 拉伸弹簧阻尼系数
	float c_shear;   // 剪切弹簧阻尼系数
	float c_bending; // 弯曲弹簧阻尼系数

	unsigned int texture; // 纹理对象

	CClothParticle *particles; // 粒子数组
	
	unsigned long num_tensile_springs; // 拉伸弹簧数量
	CClothSpring *tensile_springs;     // 拉伸弹簧数组
	unsigned long num_shear_springs;   // 剪切弹簧数量
	CClothSpring *shear_springs;       // 剪切弹簧数组
	unsigned long num_bending_springs; // 弯曲弹簧数量
	CClothSpring *bending_springs;     // 弯曲弹簧数组

	CCloth(void);
	
	void Create(float sx, float sy, unsigned long nx, unsigned long ny, 
		float tex_nx, float tex_ny, const CVector3D& offset);
	// sx, sy: x和y方向上的尺寸
	// nx, ny: x和y方向上的细分数
	// tex_nx, tex_ny: x和y方向上的纹理重复数量
	// offset: 顶点偏移
	
	virtual void Release(void);

	void ResetForces(void);
	void ApplyGravity(float gravity);
	void ApplySpringForces(void);
	void CollisionWithSphere(const CVector3D& sphere_center, float sphere_radius);
	void CollisionWithGround(float ground_height, float kfriction, float gravity);
	void UpdateParticleStates(float dt);
};

#endif
