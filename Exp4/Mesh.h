#ifndef _MESH_
#define _MESH_

#include "MathLib.h"

//
class CMeshVertex
{
public:
	CVector3D position;
	CVector3D normal; // 法线
	float texcoord[2];  // 纹理坐标
};

class CMesh
{
public:
	unsigned long num_vertices;
	unsigned long num_faces;
	
	CMeshVertex *vertices;
	unsigned long *indices;

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

};

#endif