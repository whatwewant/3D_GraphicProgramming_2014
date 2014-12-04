#include <windows.h>
#include <GL/gl.h>
#include "Skybox.h"

void CSkybox::Create(void)
{
	int i;
	float s=0.5f*size;

	static float P[8][3]={
		{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
		{-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}
	}; // 顶点坐标符号

	static unsigned char iv[6][4]={
		{5,1,2,6}, // +x
		{4,7,3,0}, // -x
		{7,6,2,3}, // +y
		{5,4,0,1}, // -y
		{4,5,6,7}, // +z
		{1,0,3,2}  // -z
	}; // 正方形表面顶点的索引

	// 设置顶点坐标
	for (i=0; i<8; ++i)
		vertices[i].Set(s*P[i][0], s*P[i][1], s*P[i][2]);

	// 设置三角形顶点索引值
	for (i=0; i<6; ++i)
	{
		int ibase=i*6; // 索引数组初始位置
		indices[ibase]=iv[i][0];   // 第0个三角形的第0个顶点
		indices[ibase+1]=iv[i][2]; // 第0个三角形的第1个顶点
		indices[ibase+2]=iv[i][1]; // 第0个三角形的第2个顶点
		indices[ibase+3]=iv[i][0]; // 第1个三角形的第0个顶点
		indices[ibase+4]=iv[i][3]; // 第1个三角形的第1个顶点
		indices[ibase+5]=iv[i][2]; // 第1个三角形的第2个顶点
	}
}

void CSkybox::Draw(void)
{
	glVertexPointer(3, GL_FLOAT, sizeof(CVector3D), &vertices[0]);
	glTexCoordPointer(3, GL_FLOAT, sizeof(CVector3D), &vertices[0]);
	// 顶点坐标也是纹理坐标

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
