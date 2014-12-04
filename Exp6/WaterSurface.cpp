#include <windows.h>
#include <GL/gl.h>
#include "WaterSurface.h"
#include <stdio.h>

void CWaterSurface::Release(void)
{
	CMesh::Release();
	if (reflection_vectors!=NULL)
		delete [] reflection_vectors;
	reflection_vectors=NULL;
}

void CWaterSurface::Draw(void)
{
	glVertexPointer(3, GL_FLOAT, sizeof(CMeshVertex), &vertices[0].position);
	glNormalPointer(GL_FLOAT, sizeof(CMeshVertex), &vertices[0].normal);
	glTexCoordPointer(3, GL_FLOAT, sizeof(CVector3D), &reflection_vectors[0]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawElements(GL_TRIANGLES, num_faces*3, GL_UNSIGNED_INT, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void CWaterSurface::Create(float sx, float sy, int nx, int ny)
{
	CreateRectangle(sx, sy, nx, ny, 1.0f, 1.0f);
	reflection_vectors=new CVector3D [num_vertices];
}

void CWaterSurface::CalculateReflectionVectors(const CVector3D &camera_pos)
{
	for (int i=0; i<num_vertices; i++) 
	{
		CVector3D V = (camera_pos - vertices[i].position) / sqrt(VectorDot(camera_pos-vertices[i].position,
																			camera_pos-vertices[i].position));
		reflection_vectors[i] = VectorDot(2*vertices[i].normal, V)*vertices[i].normal - V;
	}
}

void CWaterSurface::Animate(float time)
{
	for (int i=0; i<num_vertices; i++)
	{
		float leftValue = 0;
		float rightValue = 0;
		float zValue = 0;
		int j;
		for (j=0; j<MAX_NUM_PLANE_WAVES; j++) 
		{
			float thetai = 2 * PI * (plane_waves[j].f*time - vertices[i].position.x*plane_waves[j].Lx_rcp - 
				vertices[i].position.y*plane_waves[j].Ly_rcp);
			zValue += (plane_waves[j].A*sin(thetai + plane_waves[j].phi));
			leftValue += (plane_waves[j].A*plane_waves[j].Lx_rcp*cos(thetai + plane_waves[j].phi));
			rightValue += (plane_waves[j].A*plane_waves[j].Ly_rcp*cos(thetai + plane_waves[j].phi));
		}
		vertices[i].position.z = zValue;
		CVector3D Nt(2*PI*leftValue, 2*PI*rightValue, 1.0);
		vertices[i].normal = Nt / sqrt(VectorDot(Nt, Nt));
		//vertices[i].normal.Set(0.0f, 0.0f, 1.0f);
	}
}
