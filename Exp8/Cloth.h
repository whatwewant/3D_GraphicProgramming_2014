#ifndef _CLOTH_
#define _CLOTH_

#include "Mesh.h"

// ����������
class CClothParticle
{
public:
	float mass;  // ����
	CVector3D v; // �ٶ�
	CVector3D F; // ��
	unsigned char fixed; // �̶���־
};

// ���ϵ�����
class CClothSpring
{
public:
	float length0; // ԭʼ����
	unsigned long v0, v1; // �������ӵ��������ӵ�����
};

// ������
class CCloth: public CMesh
{
protected:
	void CalculateSpringInitialLengths
	(unsigned long num_springs, CClothSpring *springs);
	// num_springs: ��������
	// springs: ��������

	void _ApplySpringForces(float k_spring, float c_spring,
	 unsigned long num_springs, const CClothSpring *springs);
	// k_spring: ���ɵ���ϵ��
	// c_spring: ��������ϵ��
	// num_springs: ��������
	// springs: ��������

public:
	float thickness; // ���
	float density;   // �ܶ�(��λ����ϵ�����)
	
	float k_tensile; // ���쵯�ɵ���ϵ��
	float k_shear;   // ���е��ɵ���ϵ��
	float k_bending; // �������ɵ���ϵ��
	
	float c_tensile; // ���쵯������ϵ��
	float c_shear;   // ���е�������ϵ��
	float c_bending; // ������������ϵ��

	unsigned int texture; // �������

	CClothParticle *particles; // ��������
	
	unsigned long num_tensile_springs; // ���쵯������
	CClothSpring *tensile_springs;     // ���쵯������
	unsigned long num_shear_springs;   // ���е�������
	CClothSpring *shear_springs;       // ���е�������
	unsigned long num_bending_springs; // ������������
	CClothSpring *bending_springs;     // ������������

	CCloth(void);
	
	void Create(float sx, float sy, unsigned long nx, unsigned long ny, 
		float tex_nx, float tex_ny, const CVector3D& offset);
	// sx, sy: x��y�����ϵĳߴ�
	// nx, ny: x��y�����ϵ�ϸ����
	// tex_nx, tex_ny: x��y�����ϵ������ظ�����
	// offset: ����ƫ��
	
	virtual void Release(void);

	void ResetForces(void);
	void ApplyGravity(float gravity);
	void ApplySpringForces(void);
	void CollisionWithSphere(const CVector3D& sphere_center, float sphere_radius);
	void CollisionWithGround(float ground_height, float kfriction, float gravity);
	void UpdateParticleStates(float dt);
};

#endif
