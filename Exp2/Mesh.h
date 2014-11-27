#ifndef _MESH_
#define _MESH_

#include "MathLib.h"
#include <iostream>
//
class CMeshVertex
{
public:
	CVector3D position;
	CVector3D normal; // ·¨Ïß
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

	void CreateBlock (float sx, float sy, float sz);
	void CreateSphere (float radius, int nslices, int nstacks);
	void CreateCylinder (float radius, float height, int nslices, int nstacks);
	void CreateTorus (float inner_radius, float outer_radius, int nrings, int nsides);
};

#endif