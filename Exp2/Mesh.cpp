#include <windows.h>
#include <GL/gl.h>
#include "Mesh.h"

CMesh::CMesh (void)
{
	vertices = NULL;
	indices = NULL;
}

void CMesh::Release (void)
{
	if (vertices != NULL) delete [] vertices;
	if (indices != NULL) delete [] indices;

	vertices = NULL;
	indices = NULL;
}

void CMesh::Draw (void) {
	glVertexPointer (3, GL_FLOAT, sizeof(CMeshVertex), &vertices[0].position);
	glNormalPointer (GL_FLOAT, sizeof(CMeshVertex), &vertices[0].normal);

	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_NORMAL_ARRAY);
	
	glDrawElements (GL_TRIANGLES, num_faces * 3, GL_UNSIGNED_INT, indices);

	glDisableClientState (GL_NORMAL_ARRAY);
	glDisableClientState (GL_VERTEX_ARRAY);
}

void CMesh::CreateBlock (float sx, float sy, float sz)
{
	float sx_h = 0.5f * sx;
	float sy_h = 0.5f * sy;
	float sz_h = 0.5f * sz;

	static float P[8][3] = {
		{-1, -1, -1},
		{1, -1, -1},
		{1, 1, -1},
		{-1, 1, -1},
		{-1, -1, 1},
		{1, -1, 1},
		{1, 1, 1},
		{-1, 1, 1}
	}; // 

	static float N[6][3] = {
		{1, 0, 0},	// +x
		{-1, 0, 0}, // -x
		{0, 1, 0}, // +y
		{0, -1, 0},	// -y 
		{0, 0, 1},	// +z
		{0, 0, -1}	// -z
	};

	static int iv[6][4] = {
		{5, 1, 2, 6}, // +x
		{4, 7, 3, 0}, // -x
		{7, 6, 2, 3}, // +y
		{5, 4, 2, 3}, // -y
		{4, 5, 6, 7}, // +z
		{1, 0, 3, 2}, // -z
	};

	// Dynamic Memory

	num_vertices = 24;
	num_faces = 12;
	vertices = new CMeshVertex [num_vertices];
	indices = new unsigned long [num_faces * 3];

	int i, k;

	for (i=0; i<6; i++) { // all the surfaces of rectangle
		// set vertice and normal
		int vbase = i * 4; // position of vertex array
		for (k=0; k < 4; k++) 
		{
			CMeshVertex * pv = &vertices[vbase + k]; // 
			float * ps = P[iv[i][k]];
			pv->position.Set (sx_h*ps[0], sy_h*ps[1], sz_h*ps[2]);

			memcpy(&pv->normal, N[i], sizeof(float) * 3);
		}
		// 
		int ibase = i * 6;
		indices[ibase] = vbase;
		indices[ibase+1] = vbase + 1;
		indices[ibase+2] = vbase + 2;
		indices[ibase+3] = vbase;
		indices[ibase+4] = vbase + 2;
		indices[ibase+5] = vbase + 3;
	}
}

void CMesh::CreateSphere(float radius, int nslices, int nstacks)
{
	int i, j;
	double theta, phi, ctheta, stheta, cphi, sphi;
	double dtheta=(PI+PI)/(double)nslices;
	double dphi=PI/(double)nstacks;

	// ��̬�����ڴ�
	num_vertices=(nslices+1)*(nstacks+1);
	num_faces=2*nslices*nstacks;
	vertices=new CMeshVertex [num_vertices];
	indices=new unsigned long [num_faces*3];

	// ���ö�������ͷ���
	int iv=0; // ��������λ��
	for (j=0; j<=nstacks; ++j) // ѭ���������ڵ���
	{
		phi=-0.5*PI+(double)j*dphi; // ����phi��
		cphi=cos(phi);
		sphi=sin(phi);

		for (i=0; i<=nslices; ++i) // ѭ���������ڵ���
		{
			theta=(double)i*dtheta; // ����theta��
			ctheta=cos(theta);
			stheta=sin(theta);

			CMeshVertex *pv=&vertices[iv]; // ָ�򶥵��ָ��
			
			// ����ķ���
			pv->normal.x=(float)(cphi*ctheta);
			pv->normal.y=(float)(cphi*stheta);
			pv->normal.z=(float)sphi;

			// ���������
			pv->position=radius*pv->normal;

			++iv; // ������һ������
		}
	}

	// ���������ζ�������ֵ
	int k=0; // ��������λ��
	for (j=0; j<nstacks; ++j) // ѭ���ı������ڵ���
	{
		int vbase=j*(nslices+1); // ��j�е�0���ı������½Ƕ��������ֵ

		for (i=0; i<nslices; ++i, ++vbase) // ѭ���ı������ڵ���
		{
			indices[k]=vbase;             // ��0�������εĵ�0������
			indices[k+1]=vbase+1;         // ��0�������εĵ�1������
			indices[k+2]=vbase+nslices+2; // ��0�������εĵ�2������
			indices[k+3]=vbase;           // ��1�������εĵ�0������
			indices[k+4]=vbase+nslices+2; // ��1�������εĵ�1������
			indices[k+5]=vbase+nslices+1; // ��1�������εĵ�2������

			k+=6; // ��������������
		}
	}
}


void CMesh::CreateCylinder (float radius, float height, int nslices, int nstacks) 
{
	// ���� + ���� + ����
	/*
	num_vertices = (nslices + 1) * (nstacks + 1);
	num_faces = 2 * nslices * nstacks;
	vertices = new CMeshVertex [num_vertices];
	indices = new unsigned long [num_faces * 3];*/
	int i, j;
	double theta, phe, ctheta, stheta, cphi, sphi;
	double dtheta=(PI+PI)/(double)nslices;
	double dphe= height / (double)nstacks;

	// ��̬�����ڴ�
	num_vertices=(nslices+1)*(nstacks+1) + 2 * (nslices + 2); // �ܳ��ϵ�dot: nslice + 1 - 1 = nslice; ���ģ�1 
	num_faces=2*nslices*nstacks + 2 * nslices;
	vertices=new CMeshVertex [num_vertices];
	indices=new unsigned long [num_faces*3];

	// ���ö�������ͷ���
	int iv=0; // ��������λ��
	for (j=0; j<=nstacks; ++j) // ѭ���������ڵ���
	{
		phe=-0.5*height + (double)j*dphe; // ����phi��
		//cphi=cos(phi);
		//sphi=sin(phi);

		for (i=0; i<=nslices; ++i) // ѭ���������ڵ���
		{
			theta=(double)i*dtheta; // ����theta��
			ctheta=cos(theta);
			stheta=sin(theta);

			CMeshVertex *pv=&vertices[iv]; // ָ�򶥵��ָ��
			
			// ����ķ���
			pv->normal.x=(float)ctheta;
			pv->normal.y=(float)stheta;
			pv->normal.z=(float)0;

			// ���������
			pv->position.x = (float)radius * pv->normal.x;
			pv->position.y = (float)radius * pv->normal.y;
			pv->position.z = (float)phe;

			++iv; // ������һ������
		}
	}

	// ����͵���
	for (i=0; i<=nslices; ++i) // ѭ���������ڵ���
	{
		theta=(double)i*dtheta; // ����theta��
		ctheta=cos(theta);
		stheta=sin(theta);

		CMeshVertex *pv=&vertices[iv]; // ָ�򶥵��ָ��
			
		// ����ķ���
		pv->normal.x=0.0;
		pv->normal.y=0.0;
		pv->normal.z=1.0;

		// ���������
		pv->position.x = (float)(radius * (float)ctheta);
		pv->position.y = (float)radius * (float)stheta;
		pv->position.z = (float)(height / 2.0);

		++iv; // ������һ������
	}
	CMeshVertex *pv=&vertices[iv++]; // ָ�򶥵��ָ��
	pv->position.x = 0;
	pv->position.y = 0;
	pv->position.z = float(height/2.0);
	pv->normal.x=0.0;
	pv->normal.y=0.0;
	pv->normal.z=1.0;

	for (i=0; i<=nslices; ++i) // ѭ���������ڵ���
	{
		theta=(double)i*dtheta; // ����theta��
		ctheta=cos(theta);
		stheta=sin(theta);

		CMeshVertex *pv=&vertices[iv]; // ָ�򶥵��ָ��
			
		// ����ķ���
		pv->normal.x=0.0;
		pv->normal.y=0.0;
		pv->normal.z=-1.0;

		// ���������
		pv->position.x = radius * (float)ctheta;
		pv->position.y = radius * (float)stheta;
		pv->position.z = float(-height / 2.0);

		++iv; // ������һ������
	}
	pv=&vertices[iv++]; // ָ�򶥵��ָ��
	pv->position.x = 0;
	pv->position.y = 0;
	pv->position.z = float(-height/2.0);
	pv->normal.x=0.0;
	pv->normal.y=0.0;
	pv->normal.z=-1.0;

	// ���������ζ�������ֵ
	int k=0; // ��������λ��
	for (j=0; j<nstacks; ++j) // ѭ���ı������ڵ���
	{
		int vbase=j*(nslices+1); // ��j�е�0���ı������½Ƕ��������ֵ

		for (i=0; i<nslices; ++i, ++vbase) // ѭ���ı������ڵ���
		{
			indices[k]=vbase;             // ��0�������εĵ�0������
			indices[k+1]=vbase+1;         // ��0�������εĵ�1������
			indices[k+2]=vbase+nslices+2; // ��0�������εĵ�2������
			indices[k+3]=vbase;           // ��1�������εĵ�0������
			indices[k+4]=vbase+nslices+2; // ��1�������εĵ�1������
			indices[k+5]=vbase+nslices+1; // ��1�������εĵ�2������

			k+=6; // ��������������
		}
	}

	// ����͵�������
	int vbase = (nslices+1)*(nstacks+1) - 1;
	for (i=0; i<nslices; ++i) // ѭ���ı������ڵ���
	{
		indices[k]=(nslices+1)*(nstacks+1) + nslices + 1;             // ��0�������εĵ�0������
		indices[k+1]=vbase+i+1;         // ��0�������εĵ�1������
		indices[k+2]=vbase+i+2; // ��0�������εĵ�2������
		
		k+=3; // ������������
	}
	vbase = (nslices+1)*(nstacks+1) + nslices+1;
	for (i=0; i<nslices; ++i) // ѭ���ı������ڵ���
	{
		// 
		indices[k]=num_vertices-1;             // ��0�������εĵ�0������
		indices[k+2]=vbase+i+1;         // ��0�������εĵ�1������
		indices[k+1]=vbase+i+2; // ��0�������εĵ�2������

		k+=3; // ������������
	}

}

void CMesh::CreateTorus (float inner_radius, float outer_radius, int nrings, int nsides) 
{
	int i, j;
	double theta, phi, ctheta, stheta, cphi, sphi;
	double dtheta=(PI+PI)/(double)nrings;
	double dphi=(PI+PI)/(double)nsides;

	num_vertices = (nsides + 1) * (nrings + 1);
	num_faces = 2 * nsides * nrings;
	vertices = new CMeshVertex [num_vertices];
	indices = new unsigned long [num_faces * 3];

	// ���ö�������ͷ���
	int iv=0; // ��������λ��
	for (j=0; j<=nsides; ++j)
	{
		phi=-0.5*PI+(double)j*dphi; // ����phi��
		cphi=cos(phi);
		sphi=sin(phi);
		
		for(i=0; i<=nrings; ++i)
		{
			theta=(double)i*dtheta; // ����theta��
			ctheta=cos(theta);
			stheta=sin(theta);

			CMeshVertex *pv = &vertices[iv]; // ָ�򶥵��ָ��
			
			// ����ķ���
			pv->normal.x=(float)(cphi*ctheta);
			pv->normal.y=(float)(cphi*stheta);
			pv->normal.z=(float)sphi;

			// ���������
			pv->position.x = (float)((outer_radius + inner_radius * cphi) * ctheta);
			pv->position.y = (float)((outer_radius + inner_radius * cphi) * stheta);
			pv->position.z = (float)(inner_radius * sphi);

			++iv; // ������һ������
		}
	}

	// ���������ζ�������ֵ
	int k=0; // ��������λ��
	for (j=0; j<nsides; ++j) // ѭ���ı������ڵ���
	{
		int vbase=j*(nrings+1); // ��j�е�0���ı������½Ƕ��������ֵ

		for (i=0; i<nrings; ++i, ++vbase) // ѭ���ı������ڵ���
		{
			indices[k]=vbase;             // ��0�������εĵ�0������
			indices[k+1]=vbase+1;         // ��0�������εĵ�1������
			indices[k+2]=vbase+nrings+2; // ��0�������εĵ�2������
			indices[k+3]=vbase;           // ��1�������εĵ�0������
			indices[k+4]=vbase+nrings+2; // ��1�������εĵ�1������
			indices[k+5]=vbase+nrings+1; // ��1�������εĵ�2������

			k+=6; // ��������������
		}
	}
}