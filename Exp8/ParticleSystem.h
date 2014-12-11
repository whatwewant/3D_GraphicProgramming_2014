#ifndef _PARTICLE_SYSTEM_
#define _PARTICLE_SYSTEM_

#include "MathLib.h"

// ������
class CParticle
{
public:
	CVector3D x;     // λ��
	CVector3D v;     // �ٶ�
	float size;      // ��С
	float lifespan;  // ����
	float age;       // ����
	unsigned char active; // �����־
};

// �����ı��ζ�����
class CParticleQuadVertex
{
public:
	CVector3D position; // λ��
	float texcoord[2];  // ��������
};

// ����ϵͳ��
class CParticleSystem
{
protected:
	int num_particles;         // ����������
	
	CParticle *particles;  // ��������
	CParticleQuadVertex *quad_vertices; // �����ı��ζ�������

public:
	float color[4];       // ��ɫ
	unsigned int texture; // ����
	float gravity;        // �������ٶ�

	CParticleSystem(void);
	~CParticleSystem(void) {Release();}

	void Init(int _num_particles);
	void Release(void);

	void Emit(int n);
	void UpdateParticleStates(float dt);
	void Draw(const CVector3D& camera_right_dir, const CVector3D& camera_up_dir);
};

#endif
