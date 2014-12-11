#include <windows.h>
#include <GL/gl.h>
#include <time.h>
#include "ParticleSystem.h"
#include <stdio.h>

CParticleSystem::CParticleSystem(void)
{
	num_particles=0;
	particles=NULL;
	quad_vertices=NULL;

	color[0]=1.0f; color[1]=1.0f; color[2]=1.0f;
	color[3]=0.3f;
	texture=0;
	gravity=-9.8f;
}

void CParticleSystem::Init(int _num_particles)
{
	int i, k;

	Release();

	num_particles=_num_particles;

	// ��ʼ������
	particles=new CParticle [num_particles];
	for (i=0; i<num_particles; ++i) {
		particles[i].active=0;
	}

	// ��ʼ�������ı���
	quad_vertices=new CParticleQuadVertex [num_particles*4];
	k=0;
	for (i=0; i<num_particles; ++i)
	{
		quad_vertices[k].texcoord[0]=0.0f;
		quad_vertices[k].texcoord[1]=0.0f;
		quad_vertices[k+1].texcoord[0]=1.0f;
		quad_vertices[k+1].texcoord[1]=0.0f;
		quad_vertices[k+2].texcoord[0]=1.0f;
		quad_vertices[k+2].texcoord[1]=1.0f;
		quad_vertices[k+3].texcoord[0]=0.0f;
		quad_vertices[k+3].texcoord[1]=1.0f;

		k+=4;
	}

	srand((unsigned int)time(NULL)); // �����������
}

void CParticleSystem::Release(void)
{
	if (particles!=NULL) delete [] particles;
	if (quad_vertices!=NULL) delete [] quad_vertices;
	num_particles=0;
	particles=NULL;
	quad_vertices=NULL;
}

void CParticleSystem::Emit(int n)
{
	int i, counter;
	double rand_max_rcp=1.0/(double)RAND_MAX;

	counter=n;
	for (i=0; i<num_particles; ++i)
	if (!particles[i].active)
	{
		// ��������
		particles[i].active=1;

		// ��ʼ������λ��
		double r=1.0*(double)rand()*rand_max_rcp;
		double theta=2.0*PI*(double)rand()*rand_max_rcp;
		particles[i].x.x=(float)(r*cos(theta));
		particles[i].x.y=(float)(r*sin(theta));
		particles[i].x.z=0.0f;

		// ��ʼ�������ٶ�
		double v=10.0+2.0*((double)rand()*rand_max_rcp-0.5);
		double phi=PI*(0.5-0.1*(double)rand()*rand_max_rcp);
		particles[i].v.x=(float)(v*cos(phi)*cos(theta));
		particles[i].v.y=(float)(v*cos(phi)*sin(theta));
		particles[i].v.z=(float)(v*sin(phi));

		// ��ʼ�����Ӵ�С
		particles[i].size=(float)(0.4+0.1*((double)rand()*rand_max_rcp));

		// ��ʼ����������������
		particles[i].lifespan=(float)(2.0+1.0*((double)rand()*rand_max_rcp));
		particles[i].age=0.0f;

		--counter;
		if (counter<=0) break;
	}
}

void CParticleSystem::UpdateParticleStates(float dt)
{
	for (int i=0; i<num_particles; ++i)
	if (particles[i].active)
	{
		// ʹ��ŷ�����ָ��������ٶȺ�λ��
		particles[i].v += CVector3D(0.0f, 0.0f, dt * gravity);
		particles[i].x += dt * particles[i].v;
		if (particles[i].x.z <= 0.0f) {
			particles[i].x.z = 0.0f; 
			particles[i].v.z = - particles[i].v.z;
		}

		// ������������
		particles[i].age += dt;

		// ������������
		if (particles[i].age >= particles[i].lifespan) {
			particles[i].active = ! particles[i].active;
		} else {
			// 
		}
	}
}

void CParticleSystem::Draw
(const CVector3D& camera_right_dir, const CVector3D& camera_up_dir)
{
	int i, nv;

	// ���������ı��ζ�������
	nv=0;
	for (i=0; i<num_particles; ++i)
	if (particles[i].active)
	{
		float s=0.5f*particles[i].size;
		CVector3D vx=s*camera_right_dir, vy=s*camera_up_dir;

		// �����ı��ζ�������
		quad_vertices[nv].position=particles[i].x-vx-vy;
		quad_vertices[nv+1].position=particles[i].x+vx-vy;
		quad_vertices[nv+2].position=particles[i].x+vx+vy;
		quad_vertices[nv+3].position=particles[i].x-vx+vy;

		nv+=4;
	}

	// ���������ı���
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glVertexPointer(3, GL_FLOAT, sizeof(CParticleQuadVertex), 
		&quad_vertices[0].position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(CParticleQuadVertex), 
		quad_vertices[0].texcoord);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4fv(color);
	glDrawArrays(GL_QUADS, 0, nv);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);
}
