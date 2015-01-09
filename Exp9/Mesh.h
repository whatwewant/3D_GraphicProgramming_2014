#ifndef _MESH_
#define _MESH_

#include "MathLib.h"

// 网格顶点类
class CMeshVertex
{
public:
	CVector3D position; // 位置
	CVector3D normal;   // 法线
	float texcoord[2];  // 纹理坐标
};

// 网格类
class CMesh
{
public:
	unsigned long num_vertices; // 顶点数量
	unsigned long num_faces;    // 三角形数量
	
	CMeshVertex   *vertices; // 顶点数组
	unsigned long *indices;  // 索引数组

	CMesh(void);
	virtual ~CMesh(void) { Release(); }
	virtual void Release(void);

	virtual void Draw(void);

	void CreateBlock(float sx, float sy, float sz,
		float tex_nx, float tex_ny, float tex_nz);
	void CreateSphere(float radius, int nslices, int nstacks,
		float tex_ntheta, float tex_nphi);
	void CreateCylinder(float radius, float height, int nslices, int nstacks,
		float tex_ntheta, float tex_nz, float tex_nr);
	void CreateTorus(float inner_radius, float outer_radius, int nrings, int nsides,
		float tex_ntheta, float tex_nphi);
	void CreateRectangle(float sx, float sy, int nx, int ny, 
		float tex_nx, float tex_ny);
	void CalculateVertexAverageNormals(void);
	void CreateTerrainFromHeightMap(
		float sx, float sy, float tex_nx, float tex_ny,
		const char *heightmap_file_name,
		float height_offset, float height_scaling);


};

#endif
