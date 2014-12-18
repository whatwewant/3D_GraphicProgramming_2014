#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Cloth.h"

void CCloth::CalculateSpringInitialLengths
(unsigned long num_springs, CClothSpring *springs)
// num_springs: ��������
// springs: ��������
{
	unsigned long i;
	CVector3D L;
	for (i=0; i<num_springs; ++i)
	{
		L=vertices[springs[i].v0].position
			-vertices[springs[i].v1].position;
		springs[i].length0=L.Length();
	}
}

void CCloth::_ApplySpringForces(float k_spring, float c_spring,
 unsigned long num_springs, const CClothSpring *springs)
// k_spring: ���ɵ���ϵ��
// c_spring: ��������ϵ��
// num_springs: ��������
// springs: ��������
{
	float k = k_spring;
	float c = c_spring;
	for (unsigned long i=0; i<num_springs; ++i)
	{
		float l0 = springs[i].length0;
		CVector3D L=vertices[springs[i].v1].position
					-vertices[springs[i].v0].position;
		float l = L.Length();
		CVector3D d = 1.0 / l * L;
		

		// ���㵯����
		CVector3D Fe01 = k * (l-l0) * d;
		CVector3D Fe10 = -Fe01;

		// ����������
		CVector3D Fd01 = c * VectorDot((particles[springs[i].v1].v - particles[springs[i].v0].v), d) * d;
		CVector3D Fd10 = -Fd01;

		// �����������������ۼӵ�������
		
		particles[springs[i].v0].F += (Fe01 + Fd01);
		particles[springs[i].v1].F += (Fe10 + Fd10);
	}
}

CCloth::CCloth(void)
{
	particles=NULL;
	tensile_springs=NULL;
	shear_springs=NULL;
	bending_springs=NULL;

	thickness=0.01f;
	density=1.0f;
	k_tensile=100.0f;
	k_shear=1.0f;
	k_bending=0.01f;

	c_tensile=0.1f;
	c_shear=0.1f;
	c_bending=0.1f;
}

void CCloth::Create(float sx, float sy, unsigned long nx, unsigned long ny, 
	float tex_nx, float tex_ny, const CVector3D& offset)
// sx, sy: x��y�����ϵĳߴ�
// nx, ny: x��y�����ϵ�ϸ����
// tex_nx, tex_ny: x��y�����ϵ������ظ�����
// offset: ����ƫ��
{
	unsigned long i, j, k, ibase;

	// ��������������
	CreateRectangle(sx, sy, nx, ny, tex_nx, tex_ny);

	// ʩ�Ӷ���ƫ��
	for (i=0; i<num_vertices; ++i)
		vertices[i].position+=offset;

	// ������������洢�ռ�
	particles=new CClothParticle [num_vertices];

	// ��ʼ�����Ӳ���
	float pmass=density*sx*sy/(float)num_vertices;
	for (i=0; i<num_vertices; ++i)
	{
		particles[i].fixed=0;
		particles[i].mass=pmass;
		particles[i].v.Set(0.0f, 0.0f, 0.0f);
		particles[i].F.Set(0.0f, 0.0f, 0.0f);
	}

	// ��ʼ�����쵯��
	num_tensile_springs=nx*(ny+1)+ny*(nx+1);
	tensile_springs=new CClothSpring [num_tensile_springs];
	k=0;
	for (j=0; j<=ny; ++j)
	{
		ibase=j*(nx+1);
		for (i=0; i<nx; ++i)
		{
			tensile_springs[k].v0=ibase+i;
			tensile_springs[k].v1=ibase+i+1;
			++k;
		}
	}

	for (j=0; j<ny; ++j)
	{
		ibase=j*(nx+1);
		for (i=0; i<=nx; ++i)
		{
			tensile_springs[k].v0=ibase+i;
			tensile_springs[k].v1=ibase+i+nx+1;
			++k;
		}
	}

	// ��ʼ�����е���
	num_shear_springs=nx*ny*2;
	shear_springs=new CClothSpring [num_shear_springs];
	k=0;
	for (j=0; j<ny; ++j)
	{
		ibase=j*(nx+1);
		for (i=0; i<nx; ++i)
		{
			shear_springs[k].v0=ibase+i;
			shear_springs[k].v1=ibase+i+nx+2;
			++k;
			shear_springs[k].v0=ibase+i+nx+1;
			shear_springs[k].v1=ibase+i+1;
			++k;
		}
	}

	// ��ʼ����������
	num_bending_springs=(nx-1)*(ny+1)+(ny-1)*(nx+1);
	bending_springs=new CClothSpring [num_bending_springs];
	k=0;
	for (j=0; j<=ny; ++j)
	{
		ibase=j*(nx+1);
		for (i=0; i<nx-1; ++i)
		{
			bending_springs[k].v0=ibase+i;
			bending_springs[k].v1=ibase+i+2;
			++k;
		}
	}

	for (j=0; j<ny-1; ++j)
	{
		ibase=j*(nx+1);
		for (i=0; i<=nx; ++i)
		{
			bending_springs[k].v0=ibase+i;
			bending_springs[k].v1=ibase+i+2*(nx+1);
			++k;
		}
	}

	// �������е��ɵĳ�ʼ����
	CalculateSpringInitialLengths(num_tensile_springs, tensile_springs);
	CalculateSpringInitialLengths(num_shear_springs, shear_springs);
	CalculateSpringInitialLengths(num_bending_springs, bending_springs);
}

void CCloth::Release(void)
{
	CMesh::Release();

	if (particles!=NULL) delete [] particles;
	if (tensile_springs!=NULL) delete [] tensile_springs;
	if (shear_springs!=NULL) delete [] shear_springs;
	if (bending_springs!=NULL) delete [] bending_springs;

	particles=NULL;
	tensile_springs=NULL;
	shear_springs=NULL;
	bending_springs=NULL;
}

void CCloth::ResetForces(void)
{
	for (unsigned long i=0; i<num_vertices; ++i)
		particles[i].F.Set(0.0f, 0.0f, 0.0f);
}

void CCloth::ApplyGravity(float gravity)
{
	for (unsigned long i=0; i<num_vertices; ++i)
		particles[i].F -= particles[i].mass * gravity;
}

void CCloth::ApplySpringForces(void)
{
	_ApplySpringForces(k_tensile, c_tensile, num_tensile_springs, tensile_springs);
	_ApplySpringForces(k_shear, c_shear, num_shear_springs, shear_springs);
	_ApplySpringForces(k_bending, c_bending, num_bending_springs, bending_springs);
}

void CCloth::CollisionWithSphere(const CVector3D& sphere_center, float sphere_radius)
{
	for (unsigned long i=0; i<num_vertices; ++i)
	{
		float d = (vertices[i].position - sphere_center).Length();		
		
		if (d < sphere_radius + thickness/2.0)
		{
			CVector3D n = 1.0 / d * (vertices[i].position - sphere_center);
			vertices[i].position = sphere_center + (sphere_radius + thickness/2.0 ) * n;
			particles[i].v -= VectorDot(particles[i].v, n) * n;
		}
		
	}		
}

void CCloth::CollisionWithGround(float ground_height, float kfriction, float gravity)
{
	for (unsigned long i=0; i<num_vertices; ++i)
	{
		
		if (vertices[i].position.z < ground_height + thickness/2.0)
		{
			vertices[i].position.z = ground_height + thickness/2.0;
			particles[i].v.z =0;

			particles[i].v.Normalize();
			particles[i].F += - kfriction * particles[i].mass * gravity * particles[i].v;
		}		
		
	}
}

void CCloth::UpdateParticleStates(float dt)
{
	for (unsigned long i=0; i<num_vertices; ++i)
	{
		// ��������Ƿ�̶�
		if (particles[i].fixed) continue;

		// ���������ٶȺ�λ��
		particles[i].v += dt * particles[i].F / particles[i].mass;
		vertices[i].position += dt * particles[i].v;
	}
}
