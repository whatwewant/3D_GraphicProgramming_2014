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
	}; // 顶点坐标符号

	static float N[6][3]={
		{ 1, 0, 0}, // +x
		{-1, 0, 0}, // -x
		{ 0, 1, 0}, // +y
		{ 0,-1, 0}, // -y
		{ 0, 0, 1}, // +z
		{ 0, 0,-1}  // -z
	}; // 长方形表面的法线

	static int iv[6][4]={
		{5,1,2,6}, // +x
		{4,7,3,0}, // -x
		{7,6,2,3}, // +y
		{5,4,0,1}, // -y
		{4,5,6,7}, // +z
		{1,0,3,2}  // -z
	}; // 长方形表面顶点的坐标索引

	float tex_n[6][2]={
		{tex_nz, tex_ny}, // +x
		{tex_nz, tex_ny}, // -x
		{tex_nx, tex_nz}, // +y
		{tex_nx, tex_nz}, // -y
		{tex_nx, tex_ny}, // +z
		{tex_nx, tex_ny}  // -z
	}; // 长方形表面横向和纵向的纹理重复数量

	// 动态分配内存
	num_vertices=24;
	num_faces=12;
	vertices=new CMeshVertex [num_vertices];
	indices=new unsigned long [num_faces*3];

	int i, k;
	for (i=0; i<6; ++i) // 循环所有的长方形表面
	{
		// 设置顶点坐标和法线
		int vbase=i*4; // 顶点数组初始位置
		for (k=0; k<4; ++k)
		{
			CMeshVertex *pv=&vertices[vbase+k]; // 指向第i个面的第k个顶点的指针
			float *ps=P[iv[i][k]]; // 指向第i个面的第k个顶点的坐标符号的指针
			pv->position.set(sx_h*ps[0], sy_h*ps[1], sz_h*ps[2]); // 顶点坐标
			memcpy(&pv->normal, N[i], sizeof(float)*3); // 顶点法线
		}

		// 设置三角形顶点索引值
		int ibase=i*6; // 索引数组初始位置
		indices[ibase]=vbase;     // 第0个三角形的第0个顶点
		indices[ibase+1]=vbase+1; // 第0个三角形的第1个顶点
		indices[ibase+2]=vbase+2; // 第0个三角形的第2个顶点
		indices[ibase+3]=vbase;   // 第1个三角形的第0个顶点
		indices[ibase+4]=vbase+2; // 第1个三角形的第1个顶点
		indices[ibase+5]=vbase+3; // 第1个三角形的第2个顶点

		// 设置顶点纹理坐标
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

	// 动态分配内存
	num_vertices=(nslices+1)*(nstacks+1);
	num_faces=2*nslices*nstacks;
	vertices=new CMeshVertex [num_vertices];
	indices=new unsigned long [num_faces*3];

	// 设置顶点坐标和法线
	int iv=0; // 顶点数组位置
	for (j=0; j<=nstacks; ++j) // 循环顶点所在的行
	{
		phi=-0.5*PI+(double)j*dphi; // 计算phi角
		cphi=cos(phi);
		sphi=sin(phi);
		float texcoord_t=((float)j/(float)nstacks)*tex_nphi; // 纹理坐标t

		for (i=0; i<=nslices; ++i) // 循环顶点所在的列
		{
			theta=(double)i*dtheta; // 计算theta角
			ctheta=cos(theta);
			stheta=sin(theta);

			CMeshVertex *pv=&vertices[iv]; // 指向顶点的指针
			
			// 顶点的法线
			pv->normal.x=(float)(cphi*ctheta);
			pv->normal.y=(float)(cphi*stheta);
			pv->normal.z=(float)sphi;

			// 顶点的坐标
			pv->position=radius*pv->normal;

			// 顶点的纹理坐标
			pv->texcoord[0]=((float)i/(float)nslices)*tex_ntheta;
			pv->texcoord[1]=texcoord_t;

			++iv; // 跳到下一个顶点
		}
	}

	// 设置三角形顶点索引值
	int k=0; // 索引数组位置
	for (j=0; j<nstacks; ++j) // 循环四边形所在的行
	{
		int vbase=j*(nslices+1); // 第j行第0列四边形左下角顶点的索引值

		for (i=0; i<nslices; ++i, ++vbase) // 循环四边形所在的列
		{
			indices[k]=vbase;             // 第0个三角形的第0个顶点
			indices[k+1]=vbase+1;         // 第0个三角形的第1个顶点
			indices[k+2]=vbase+nslices+2; // 第0个三角形的第2个顶点
			indices[k+3]=vbase;           // 第1个三角形的第0个顶点
			indices[k+4]=vbase+nslices+2; // 第1个三角形的第1个顶点
			indices[k+5]=vbase+nslices+1; // 第1个三角形的第2个顶点

			k+=6; // 跳过两个三角形
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

	// 设置顶点坐标和法线
	int iv=0; // 顶点数组位置
	for (j=0; j<=nstacks; ++j) // 循环顶点所在的行
	{
		float texcoord_t=((float)j/(float)nstacks)*tex_nz; // 纹理坐标t

		for (i=0; i<=nslices; ++i) // 循环顶点所在的列
		{
			theta=(double)i*dtheta; // 计算theta角
			ctheta=cos(theta);
			stheta=sin(theta);

			CMeshVertex *pv=&vertices[iv]; // 指向顶点的指针
			
			// 顶点的法线
			pv->normal.x=(float)ctheta;
			pv->normal.y=(float)stheta;
			pv->normal.z=(float)0;

			// 顶点的坐标
			pv->position.x=radius*pv->normal.x;
			pv->position.y=radius*pv->normal.y;
			pv->position.z=-0.5f*height+j*sh;

			// 顶点的纹理坐标
			pv->texcoord[0]=((float)i/(float)nslices)*tex_ntheta;
			pv->texcoord[1]=texcoord_t;

			++iv; // 跳到下一个顶点
		}
	}

	CMeshVertex *pv=&vertices[iv];
	pv->normal.x=(float)0;
	pv->normal.y=(float)0;
	pv->normal.z=(float)1;

	// 顶点的坐标
	pv->position.x=0;
	pv->position.y=0;
	pv->position.z=0.5f*height;

	// 顶点的纹理坐标
	pv->texcoord[0]=(float)((1.0/2.0)*(pv->position.x/radius+1)*tex_nr);
	pv->texcoord[1]=(float)((1.0/2.0)*(pv->position.y/radius+1)*tex_nr);
	++iv;

	for (i=0; i<=nslices; ++i) // 循环顶点所在的列
	{
		theta=(double)i*dtheta; // 计算theta角
		ctheta=cos(theta);
		stheta=sin(theta);

		CMeshVertex *pv=&vertices[iv]; // 指向顶点的指针
			
		// 顶点的法线
		pv->normal.x=(float)0;
		pv->normal.y=(float)0;
		pv->normal.z=(float)1;

		// 顶点的坐标
		pv->position.x=radius*(float)ctheta;
		pv->position.y=radius*(float)stheta;
		pv->position.z=0.5f*height;

		// 顶点的纹理坐标
		pv->texcoord[0]=(float)((1.0/2.0)*(pv->position.x/radius+1)*tex_nr);
		pv->texcoord[1]=(float)((1.0/2.0)*(pv->position.y/radius+1)*tex_nr);

		++iv; // 跳到下一个顶点
	}

	pv=&vertices[iv];
	pv->normal.x=(float)0;
	pv->normal.y=(float)0;
	pv->normal.z=(float)(-1);

	// 顶点的坐标
	pv->position.x=0;
	pv->position.y=0;
	pv->position.z=-0.5f*height;

	// 顶点的纹理坐标
	pv->texcoord[0]=(float)((1.0/2.0)*(pv->position.x/radius+1)*tex_nr);
	pv->texcoord[1]=(float)((1.0/2.0)*(pv->position.y/radius+1)*tex_nr);
	++iv;

	for (i=0; i<=nslices; ++i) // 循环顶点所在的列
	{
	    theta=(double)i*dtheta; // 计算theta角
		ctheta=cos(theta);
		stheta=sin(theta);
			
		CMeshVertex *pv=&vertices[iv];

		// 顶点的法线
		pv->normal.x=(float)0;
		pv->normal.y=(float)0;
		pv->normal.z=(float)(-1);

		// 顶点的坐标
		pv->position.x=radius*(float)ctheta;
		pv->position.y=radius*(float)stheta;
		pv->position.z=-0.5f*height;

		// 顶点的纹理坐标
		pv->texcoord[0]=(float)((1.0/2.0)*(pv->position.x/radius+1)*tex_nr);
		pv->texcoord[1]=(float)((1.0/2.0)*(pv->position.y/radius+1)*tex_nr);

		++iv; // 跳到下一个顶点
	}

	// 设置三角形顶点索引值
	int k=0; // 索引数组位置
	for (j=0; j<nstacks; ++j) // 循环四边形所在的行
	{
		int vbase=j*(nslices+1); // 第j行第0列四边形左下角顶点的索引值

		for (i=0; i<nslices; ++i, ++vbase) // 循环四边形所在的列
		{
			indices[k]=vbase;             // 第0个三角形的第0个顶点
			indices[k+1]=vbase+1;         // 第0个三角形的第1个顶点
			indices[k+2]=vbase+nslices+2; // 第0个三角形的第2个顶点
			indices[k+3]=vbase;           // 第1个三角形的第0个顶点
			indices[k+4]=vbase+nslices+2; // 第1个三角形的第1个顶点
			indices[k+5]=vbase+nslices+1; // 第1个三角形的第2个顶点

			k+=6; // 跳过两个三角形
		}
	}
	int vbase = (nslices+1)*(nstacks+1);
	for (i=0; i<nslices; ++i) // 循环四边形所在的列
	{
		indices[k]=(nslices+1)*(nstacks+1);             // 第0个三角形的第0个顶点
		indices[k+1]=vbase+i+1;         // 第0个三角形的第1个顶点
		indices[k+2]=vbase+i+2; // 第0个三角形的第2个顶点
		
		k+=3; // 跳过个三角形
	}
	vbase = (nslices+1)*(nstacks+1)+nslices+2;
	for (i=0; i<nslices; ++i) // 循环四边形所在的列
	{
		// 
		indices[k]=num_vertices-nslices-2;             // 第0个三角形的第0个顶点
		indices[k+2]=vbase+i+1;         // 第0个三角形的第1个顶点
		indices[k+1]=vbase+i+2; // 第0个三角形的第2个顶点

		k+=3; // 跳过个三角形
	}
}

void CMesh::CreateTorus(float inner_radius, float outer_radius, int nrings, int nsides,
		float tex_ntheta, float tex_nphi)
{
	int i, j;
	double theta, phi, ctheta, stheta, cphi, sphi;
	double dtheta=(PI+PI)/(double)nrings;
	double dphi=(PI+PI)/(double)nsides;

	// 动态分配内存
	num_vertices=(nrings+1)*(nsides+1);
	num_faces=2*nrings*nsides;
	vertices=new CMeshVertex [num_vertices];
	indices=new unsigned long [num_faces*3];

	// 设置顶点坐标和法线
	int iv=0; // 顶点数组位置
	for (j=0; j<=nsides; ++j) // 循环顶点所在的行
	{
		phi=(double)j*dphi; // 计算phi角
		cphi=cos(phi);
		sphi=sin(phi);
		float texcoord_t=((float)j/(float)nsides)*tex_nphi; // 纹理坐标t

		for (i=0; i<=nrings; ++i) // 循环顶点所在的列
		{
			theta=(double)i*dtheta; // 计算theta角
			ctheta=cos(theta);
			stheta=sin(theta);

			CMeshVertex *pv=&vertices[iv]; // 指向顶点的指针
			
			// 顶点的法线
			pv->normal.x=(float)(cphi*ctheta);
			pv->normal.y=(float)(cphi*stheta);
			pv->normal.z=(float)sphi;

			// 顶点的坐标
			pv->position.x=(outer_radius+inner_radius*(float)cphi)*(float)ctheta;
			pv->position.y=(outer_radius+inner_radius*(float)cphi)*(float)stheta;
			pv->position.z=inner_radius*(float)sphi;

			// 顶点的纹理坐标
			pv->texcoord[0]=((float)i/(float)nrings)*tex_ntheta;
			pv->texcoord[1]=texcoord_t;

			++iv; // 跳到下一个顶点
		}
	}

	// 设置三角形顶点索引值
	int k=0; // 索引数组位置
	for (j=0; j<nsides; ++j) // 循环四边形所在的行
	{
		int vbase=j*(nrings+1); // 第j行第0列四边形左下角顶点的索引值

		for (i=0; i<nrings; ++i, ++vbase) // 循环四边形所在的列
		{
			indices[k]=vbase;             // 第0个三角形的第0个顶点
			indices[k+1]=vbase+1;         // 第0个三角形的第1个顶点
			indices[k+2]=vbase+nrings+2; // 第0个三角形的第2个顶点
			indices[k+3]=vbase;           // 第1个三角形的第0个顶点
			indices[k+4]=vbase+nrings+2; // 第1个三角形的第1个顶点
			indices[k+5]=vbase+nrings+1; // 第1个三角形的第2个顶点

			k+=6; // 跳过两个三角形
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

	// 动态分配内存
	num_vertices=(nx+1)*(ny+1);
	num_faces=2*nx*ny;
	vertices=new CMeshVertex [num_vertices];
	indices=new unsigned long [num_faces*3];

	// 设置顶点坐标和法线
	int iv=0; // 顶点数组位置
	for (j=0; j<=ny; ++j) // 循环顶点所在的行
	{
		y=-hy+(double)j*dy; // 计算y坐标
		float texcoord_t=((float)j/(float)ny)*tex_ny; // 纹理坐标t

		for (i=0; i<=nx; ++i) // 循环顶点所在的列
		{
			CMeshVertex *pv=&vertices[iv]; // 指向顶点的指针
			
			// 顶点的法线
			pv->normal.set(0.0f, 0.0f, 1.0f);

			// 顶点的坐标
			pv->position.x=(float)(-hx+(double)i*dx);
			pv->position.y=(float)y;
			pv->position.z=0.0f;

			// 顶点的纹理坐标
			pv->texcoord[0]=((float)i/(float)nx)*tex_nx;
			pv->texcoord[1]=texcoord_t;

			++iv; // 跳到下一个顶点
		}
	}

	// 设置三角形顶点索引值
	int k=0; // 索引数组位置
	for (j=0; j<ny; ++j) // 循环长方形所在的行
	{
		int vbase=j*(nx+1); // 第j行第0列长方形左下角顶点的索引值

		for (i=0; i<nx; ++i, ++vbase) // 循环长方形所在的列
		{
			indices[k]=vbase;        // 第0个三角形的第0个顶点
			indices[k+1]=vbase+1;    // 第0个三角形的第1个顶点
			indices[k+2]=vbase+nx+2; // 第0个三角形的第2个顶点
			indices[k+3]=vbase;      // 第1个三角形的第0个顶点
			indices[k+4]=vbase+nx+2; // 第1个三角形的第1个顶点
			indices[k+5]=vbase+nx+1; // 第1个三角形的第2个顶点

			k+=6; // 跳过两个三角形
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
	// 载入高度图
	CImage img;
	if (!img.Load(heightmap_file_name)) return;
	int nx=img.width-1;   // x方向细分数
	int ny=img.height-1;  // y方向细分数

	// 构造长方形网格
	CreateRectangle(sx, sy, nx, ny, tex_nx, tex_ny);

	// 设置顶点高度
	int i, n=(nx+1)*(ny+1);
	for (i=0; i<n; ++i)
	{
		float h=(float)img.pixels[i*img.bpp];
		vertices[i].position.z=height_scaling*h+height_offset;
	}

	// 计算顶点平均法线
	CalculateVertexAverageNormals();
}


