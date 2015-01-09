#include <windows.h>
#include <GL/gl.h>
#include "Mesh.h"
#include "ImageLib.h"

CMesh::CMesh(void)
{
	vertices=NULL;
	indices=NULL;
}

void CMesh::Release(void)
{
	if (vertices!=NULL) delete [] vertices;
	if (indices!=NULL) delete [] indices;
	vertices=NULL;
	indices=NULL;
}

void CMesh::Draw(void)
{
	glVertexPointer(3, GL_FLOAT, sizeof(CMeshVertex), &vertices[0].position);
	glNormalPointer(GL_FLOAT, sizeof(CMeshVertex), &vertices[0].normal);
	glTexCoordPointer(2, GL_FLOAT, sizeof(CMeshVertex), vertices[0].texcoord);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawElements(GL_TRIANGLES, num_faces*3, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

void CMesh::CreateBlock(float sx, float sy, float sz,
	float tex_nx, float tex_ny, float tex_nz)

{
	float sx_h=0.5f*sx;
	float sy_h=0.5f*sy;
	float sz_h=0.5f*sz;

	static float P[8][3]={
		{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
		{-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}
	}; // �����������

	static float N[6][3]={
		{ 1, 0, 0}, // +x
		{-1, 0, 0}, // -x
		{ 0, 1, 0}, // +y
		{ 0,-1, 0}, // -y
		{ 0, 0, 1}, // +z
		{ 0, 0,-1}  // -z
	}; // �����α���ķ���

	static int iv[6][4]={
		{5,1,2,6}, // +x
		{4,7,3,0}, // -x
		{7,6,2,3}, // +y
		{5,4,0,1}, // -y
		{4,5,6,7}, // +z
		{1,0,3,2}  // -z
	}; // �����α��涥�����������

	float tex_n[6][2]={
		{tex_nz, tex_ny}, // +x
		{tex_nz, tex_ny}, // -x
		{tex_nx, tex_nz}, // +y
		{tex_nx, tex_nz}, // -y
		{tex_nx, tex_ny}, // +z
		{tex_nx, tex_ny}  // -z
	}; // �����α�����������������ظ�����

	// ��̬�����ڴ�
	num_vertices=24;
	num_faces=12;
	vertices=new CMeshVertex [num_vertices];
	indices=new unsigned long [num_faces*3];

	int i, k;
	for (i=0; i<6; ++i) // ѭ�����еĳ����α���
	{
		// ���ö�������ͷ���
		int vbase=i*4; // ���������ʼλ��
		for (k=0; k<4; ++k)
		{
			CMeshVertex *pv=&vertices[vbase+k]; // ָ���i����ĵ�k�������ָ��
			float *ps=P[iv[i][k]]; // ָ���i����ĵ�k�������������ŵ�ָ��
			pv->position.set(sx_h*ps[0], sy_h*ps[1], sz_h*ps[2]); // ��������
			memcpy(&pv->normal, N[i], sizeof(float)*3); // ���㷨��
		}

		// ���������ζ�������ֵ
		int ibase=i*6; // ���������ʼλ��
		indices[ibase]=vbase;     // ��0�������εĵ�0������
		indices[ibase+1]=vbase+1; // ��0�������εĵ�1������
		indices[ibase+2]=vbase+2; // ��0�������εĵ�2������
		indices[ibase+3]=vbase;   // ��1�������εĵ�0������
		indices[ibase+4]=vbase+2; // ��1�������εĵ�1������
		indices[ibase+5]=vbase+3; // ��1�������εĵ�2������

		// ���ö�����������
		vertices[vbase].texcoord[0]=0.0f;
		vertices[vbase].texcoord[1]=0.0f;
		vertices[vbase+1].texcoord[0]=tex_n[i][0];
		vertices[vbase+1].texcoord[1]=0.0f;
		vertices[vbase+2].texcoord[0]=tex_n[i][0];
		vertices[vbase+2].texcoord[1]=tex_n[i][1];
		vertices[vbase+3].texcoord[0]=0.0f;
		vertices[vbase+3].texcoord[1]=tex_n[i][1];

	}

}

void CMesh::CreateSphere(float radius, int nslices, int nstacks,
	float tex_ntheta, float tex_nphi)
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
		float texcoord_t=((float)j/(float)nstacks)*tex_nphi; // ��������t

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

			// �������������
			pv->texcoord[0]=((float)i/(float)nslices)*tex_ntheta;
			pv->texcoord[1]=texcoord_t;

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

void CMesh::CreateCylinder(float radius, float height, int nslices, int nstacks,
		float tex_ntheta, float tex_nz, float tex_nr)
{
	int i,j;
	double theta,ctheta,stheta;
	double dtheta=(2*PI)/(double)nslices;
	float sh=height/(float)nstacks;

    num_vertices=(nslices+1)*(nstacks+1)+2*(nslices+2); 
	num_faces=2*nslices*nstacks+2*nslices;
	vertices=new CMeshVertex [num_vertices];
	indices=new unsigned long [num_faces*3];

	// ���ö�������ͷ���
	int iv=0; // ��������λ��
	for (j=0; j<=nstacks; ++j) // ѭ���������ڵ���
	{
		float texcoord_t=((float)j/(float)nstacks)*tex_nz; // ��������t

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
			pv->position.x=radius*pv->normal.x;
			pv->position.y=radius*pv->normal.y;
			pv->position.z=-0.5f*height+j*sh;

			// �������������
			pv->texcoord[0]=((float)i/(float)nslices)*tex_ntheta;
			pv->texcoord[1]=texcoord_t;

			++iv; // ������һ������
		}
	}

	CMeshVertex *pv=&vertices[iv];
	pv->normal.x=(float)0;
	pv->normal.y=(float)0;
	pv->normal.z=(float)1;

	// ���������
	pv->position.x=0;
	pv->position.y=0;
	pv->position.z=0.5f*height;

	// �������������
	pv->texcoord[0]=(float)((1.0/2.0)*(pv->position.x/radius+1)*tex_nr);
	pv->texcoord[1]=(float)((1.0/2.0)*(pv->position.y/radius+1)*tex_nr);
	++iv;

	for (i=0; i<=nslices; ++i) // ѭ���������ڵ���
	{
		theta=(double)i*dtheta; // ����theta��
		ctheta=cos(theta);
		stheta=sin(theta);

		CMeshVertex *pv=&vertices[iv]; // ָ�򶥵��ָ��
			
		// ����ķ���
		pv->normal.x=(float)0;
		pv->normal.y=(float)0;
		pv->normal.z=(float)1;

		// ���������
		pv->position.x=radius*(float)ctheta;
		pv->position.y=radius*(float)stheta;
		pv->position.z=0.5f*height;

		// �������������
		pv->texcoord[0]=(float)((1.0/2.0)*(pv->position.x/radius+1)*tex_nr);
		pv->texcoord[1]=(float)((1.0/2.0)*(pv->position.y/radius+1)*tex_nr);

		++iv; // ������һ������
	}

	pv=&vertices[iv];
	pv->normal.x=(float)0;
	pv->normal.y=(float)0;
	pv->normal.z=(float)(-1);

	// ���������
	pv->position.x=0;
	pv->position.y=0;
	pv->position.z=-0.5f*height;

	// �������������
	pv->texcoord[0]=(float)((1.0/2.0)*(pv->position.x/radius+1)*tex_nr);
	pv->texcoord[1]=(float)((1.0/2.0)*(pv->position.y/radius+1)*tex_nr);
	++iv;

	for (i=0; i<=nslices; ++i) // ѭ���������ڵ���
	{
	    theta=(double)i*dtheta; // ����theta��
		ctheta=cos(theta);
		stheta=sin(theta);
			
		CMeshVertex *pv=&vertices[iv];

		// ����ķ���
		pv->normal.x=(float)0;
		pv->normal.y=(float)0;
		pv->normal.z=(float)(-1);

		// ���������
		pv->position.x=radius*(float)ctheta;
		pv->position.y=radius*(float)stheta;
		pv->position.z=-0.5f*height;

		// �������������
		pv->texcoord[0]=(float)((1.0/2.0)*(pv->position.x/radius+1)*tex_nr);
		pv->texcoord[1]=(float)((1.0/2.0)*(pv->position.y/radius+1)*tex_nr);

		++iv; // ������һ������
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
	int vbase = (nslices+1)*(nstacks+1);
	for (i=0; i<nslices; ++i) // ѭ���ı������ڵ���
	{
		indices[k]=(nslices+1)*(nstacks+1);             // ��0�������εĵ�0������
		indices[k+1]=vbase+i+1;         // ��0�������εĵ�1������
		indices[k+2]=vbase+i+2; // ��0�������εĵ�2������
		
		k+=3; // ������������
	}
	vbase = (nslices+1)*(nstacks+1)+nslices+2;
	for (i=0; i<nslices; ++i) // ѭ���ı������ڵ���
	{
		// 
		indices[k]=num_vertices-nslices-2;             // ��0�������εĵ�0������
		indices[k+2]=vbase+i+1;         // ��0�������εĵ�1������
		indices[k+1]=vbase+i+2; // ��0�������εĵ�2������

		k+=3; // ������������
	}
}

void CMesh::CreateTorus(float inner_radius, float outer_radius, int nrings, int nsides,
		float tex_ntheta, float tex_nphi)
{
	int i, j;
	double theta, phi, ctheta, stheta, cphi, sphi;
	double dtheta=(PI+PI)/(double)nrings;
	double dphi=(PI+PI)/(double)nsides;

	// ��̬�����ڴ�
	num_vertices=(nrings+1)*(nsides+1);
	num_faces=2*nrings*nsides;
	vertices=new CMeshVertex [num_vertices];
	indices=new unsigned long [num_faces*3];

	// ���ö�������ͷ���
	int iv=0; // ��������λ��
	for (j=0; j<=nsides; ++j) // ѭ���������ڵ���
	{
		phi=(double)j*dphi; // ����phi��
		cphi=cos(phi);
		sphi=sin(phi);
		float texcoord_t=((float)j/(float)nsides)*tex_nphi; // ��������t

		for (i=0; i<=nrings; ++i) // ѭ���������ڵ���
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
			pv->position.x=(outer_radius+inner_radius*(float)cphi)*(float)ctheta;
			pv->position.y=(outer_radius+inner_radius*(float)cphi)*(float)stheta;
			pv->position.z=inner_radius*(float)sphi;

			// �������������
			pv->texcoord[0]=((float)i/(float)nrings)*tex_ntheta;
			pv->texcoord[1]=texcoord_t;

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

void CMesh::CreateRectangle(float sx, float sy, int nx, int ny, 
	float tex_nx, float tex_ny)
{
	int i, j;
	double y;
	double hx=0.5*sx;
	double hy=0.5*sy;
	double dx=(double)sx/(double)nx;
	double dy=(double)sy/(double)ny;

	// ��̬�����ڴ�
	num_vertices=(nx+1)*(ny+1);
	num_faces=2*nx*ny;
	vertices=new CMeshVertex [num_vertices];
	indices=new unsigned long [num_faces*3];

	// ���ö�������ͷ���
	int iv=0; // ��������λ��
	for (j=0; j<=ny; ++j) // ѭ���������ڵ���
	{
		y=-hy+(double)j*dy; // ����y����
		float texcoord_t=((float)j/(float)ny)*tex_ny; // ��������t

		for (i=0; i<=nx; ++i) // ѭ���������ڵ���
		{
			CMeshVertex *pv=&vertices[iv]; // ָ�򶥵��ָ��
			
			// ����ķ���
			pv->normal.set(0.0f, 0.0f, 1.0f);

			// ���������
			pv->position.x=(float)(-hx+(double)i*dx);
			pv->position.y=(float)y;
			pv->position.z=0.0f;

			// �������������
			pv->texcoord[0]=((float)i/(float)nx)*tex_nx;
			pv->texcoord[1]=texcoord_t;

			++iv; // ������һ������
		}
	}

	// ���������ζ�������ֵ
	int k=0; // ��������λ��
	for (j=0; j<ny; ++j) // ѭ�����������ڵ���
	{
		int vbase=j*(nx+1); // ��j�е�0�г��������½Ƕ��������ֵ

		for (i=0; i<nx; ++i, ++vbase) // ѭ�����������ڵ���
		{
			indices[k]=vbase;        // ��0�������εĵ�0������
			indices[k+1]=vbase+1;    // ��0�������εĵ�1������
			indices[k+2]=vbase+nx+2; // ��0�������εĵ�2������
			indices[k+3]=vbase;      // ��1�������εĵ�0������
			indices[k+4]=vbase+nx+2; // ��1�������εĵ�1������
			indices[k+5]=vbase+nx+1; // ��1�������εĵ�2������

			k+=6; // ��������������
		}
	}
}

void CMesh::CalculateVertexAverageNormals(void)
{
	unsigned long i;

	for (i=0; i<num_vertices; ++i)
		vertices[i].normal.set(0.0f, 0.0f, 0.0f);

	for (i=0; i<num_faces; ++i)
	{
		unsigned long *iv=indices+i*3;
		CVector3D N=TriangleNormal(
			vertices[iv[0]].position,
			vertices[iv[1]].position,
			vertices[iv[2]].position);
		vertices[iv[0]].normal+=N;
		vertices[iv[1]].normal+=N;
		vertices[iv[2]].normal+=N;
	}

	for (i=0; i<num_vertices; ++i)
		vertices[i].normal.Normalize();
}

void CMesh::CreateTerrainFromHeightMap(
	float sx, float sy, float tex_nx, float tex_ny,
	const char *heightmap_file_name,
	float height_offset, float height_scaling)
{
	// ����߶�ͼ
	CImage img;
	if (!img.Load(heightmap_file_name)) return;
	int nx=img.width-1;   // x����ϸ����
	int ny=img.height-1;  // y����ϸ����

	// ���쳤��������
	CreateRectangle(sx, sy, nx, ny, tex_nx, tex_ny);

	// ���ö���߶�
	int i, n=(nx+1)*(ny+1);
	for (i=0; i<n; ++i)
	{
		float h=(float)img.pixels[i*img.bpp];
		vertices[i].position.z=height_scaling*h+height_offset;
	}

	// ���㶥��ƽ������
	CalculateVertexAverageNormals();
}

