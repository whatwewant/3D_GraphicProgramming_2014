#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>

#include "MD5.h"
#include "ImageLib.h"

MD5Mesh::MD5Mesh(void)
{
	vertices2=NULL; weights=NULL;
	texture_file_name[0]=0;
	texture=0;
}

void MD5Mesh::Release(void)
{
	CMesh::Release();
	if (vertices2!=NULL) delete [] vertices2;
	if (weights!=NULL) delete [] weights;
	vertices2=NULL; weights=NULL;
}

MD5Model::MD5Model(void)
{
	num_meshes=0; num_joints=0;
	meshes=NULL; bindpose_skeleton=NULL; current_skeleton=NULL;
}

void MD5Model::Release(void)
{
	if (meshes!=NULL) delete [] meshes;
	if (bindpose_skeleton!=NULL) delete [] bindpose_skeleton;
	if (current_skeleton!=NULL) delete [] current_skeleton;
	meshes=NULL; bindpose_skeleton=NULL; current_skeleton=NULL;
}

int MD5Model::Load(const char *file_name)
{
	FILE *fp=fopen(file_name, "rt");
	if (fp==NULL) return 0;

	int i, j;
	char  buf[512];   // 临时缓冲区
	int   itemp[4];   // 临时整数缓冲区
	float ftemp[6];   // 临时浮点数缓冲区
	char  ctemp[256]; // 临时字符缓冲区

	int imesh=0;  // 网格索引

	while (!feof(fp))
	{
		// 读取文件一行
		fgets(buf, sizeof(buf), fp);

		// 解析读入的文件行
		if (sscanf(buf, "numJoints %d", &itemp[0])==1)
		{
			// 分配骨架存储空间
			num_joints=itemp[0];
			bindpose_skeleton=new MD5Joint [num_joints];
			current_skeleton=new MD5Joint [num_joints];
			memset(bindpose_skeleton, 0, sizeof(MD5Joint)*num_joints);
		}
		else if (sscanf(buf, "numMeshes %d", &itemp[0])==1)
		{
			// 分配网格存储空间
			num_meshes=itemp[0];
			meshes=new MD5Mesh [num_meshes];
		}
		else if (strncmp(buf, "joints {", 8)==0)
		{
			// 读绑定姿势骨架的关节数据
			i=0;
			while (i<num_joints)
			{
				fgets(buf, sizeof(buf), fp);
				if (8==sscanf(buf, "%s %d ( %f %f %f ) ( %f %f %f )",
					ctemp, &itemp[0], &ftemp[0], &ftemp[1], &ftemp[2],
					&ftemp[3], &ftemp[4], &ftemp[5]))
				{
					MD5Joint *pj=bindpose_skeleton+i;
					pj->parent=itemp[0];
					pj->pos.x=ftemp[0]; pj->pos.y=ftemp[1]; pj->pos.z=ftemp[2];
					pj->orient.x=ftemp[3];
					pj->orient.y=ftemp[4];
					pj->orient.z=ftemp[5];
					pj->orient.CalculateScalar();
					pj->orient.w=-pj->orient.w;
					++i;
				}
			}

			// 把绑定姿势骨架数据拷贝到当前骨架数组
			memcpy(current_skeleton, bindpose_skeleton, sizeof(MD5Joint)*num_joints);
		}
		else if (strncmp(buf, "mesh {", 6)==0)
		{
			// 读取网格信息
			MD5Mesh *pm=meshes+imesh;

			while (!feof(fp))
			{
				fgets(buf, sizeof(buf), fp);
				if (buf[0]=='}') break;
				else if (strstr(buf, "shader")!=NULL)
				{
					// 读取纹理图像文件名
					i=0;
					while (buf[i]!='\"') ++i;
					++i; j=0;
					while (buf[i]!='\"')
					{
						pm->texture_file_name[j]=buf[i];
						++i; ++j;
					}
					pm->texture_file_name[j]=0;
				}
				else if (sscanf(buf, " numverts %d", &itemp[0])==1)
				{
					// 读取网格顶点数据
					pm->num_vertices=itemp[0];
					pm->vertices=new CMeshVertex [pm->num_vertices];
					pm->vertices2=new MD5Vertex [pm->num_vertices];
					i=0;
					while (i<pm->num_vertices)
					{
						fgets(buf, sizeof(buf), fp);
						if (5==sscanf(buf, " vert %d ( %f %f ) %d %d",
							&itemp[0], &ftemp[0], &ftemp[1], &itemp[1], &itemp[2]))
						{
							CMeshVertex *pv=pm->vertices+itemp[0];
							MD5Vertex *pv2=pm->vertices2+itemp[0];
							pv->texcoord[0]=ftemp[0];
							pv->texcoord[1]=1.0f-ftemp[1]; // 颠倒纹理t坐标
							pv2->start=itemp[1];
							pv2->count=itemp[2];
							++i;
						}
					}
				}
				else if (sscanf(buf, " numtris %d", &itemp[0])==1)
				{
					// 读取网格三角形数据
					pm->num_faces=itemp[0];
					pm->indices=new unsigned long [3*pm->num_faces];
					i=0;
					while (i<pm->num_faces)
					{
						fgets(buf, sizeof(buf), fp);
						if (4==sscanf(buf, " tri %d %d %d %d",
							&itemp[0], &itemp[1], &itemp[2], &itemp[3]))
						{
							unsigned long *pt=pm->indices+3*itemp[0];
							pt[0]=itemp[1]; pt[1]=itemp[3]; pt[2]=itemp[2];
							// 颠倒三角形顶点顺序
							++i;
						}
					}
				}
				else if (sscanf(buf, " numweights %d", &itemp[0])==1)
				{
					// 读取网格权重数据
					pm->num_weights=itemp[0];
					pm->weights=new MD5Weight [pm->num_weights];
					i=0;
					while (i<pm->num_weights)
					{
						fgets(buf, sizeof(buf), fp);
						if (6==sscanf(buf, " weight %d %d %f ( %f %f %f )",
							&itemp[0], &itemp[1], &ftemp[0],
							 &ftemp[1], &ftemp[2], &ftemp[3]))
						{
							MD5Weight *pw=pm->weights+itemp[0];
							pw->joint=itemp[1];
							pw->bias=ftemp[0];
							pw->pos.x=ftemp[1];
							pw->pos.y=ftemp[2];
							pw->pos.z=ftemp[3];
							++i;
						}
					}
				}
			}

			++imesh;
		}
	}

	fclose(fp);

	CalculateVertexPositions();
	CalculateVertexAverageNormals();

	return 1;
}

void MD5Model::CalculateVertexPositions(void)
{
	for(int i=0; i<num_meshes; ++i)
		for(int j=0; j<meshes[i].num_vertices; ++j)
		{
			meshes[i].vertices[j].position=CVector3D(0.0f,0.0f,0.0f);
			for(int k=0; k<meshes[i].vertices2[j].count; ++k)
			{
				meshes[i].vertices[j].position+=
					meshes[i].weights[meshes[i].vertices2[j].start+k].bias*
					(current_skeleton[meshes[i].weights[meshes[i].vertices2[j].start+k].joint].pos
					+current_skeleton[meshes[i].weights[meshes[i].vertices2[j].start+k].joint].orient.TransformPoint(meshes[i].weights[meshes[i].vertices2[j].start+k].pos));
			}
		}
}

void MD5Model::CalculateVertexAverageNormals(void)
{
	for (int i=0; i<num_meshes; ++i)
		meshes[i].CalculateVertexAverageNormals();
}

void MD5Model::InitTextures(const char *texture_path)
{
	char file_full_path[1024];

	for (int i=0; i<num_meshes; ++i)
	{
		MD5Mesh *pm=meshes+i;

		strcpy(file_full_path, texture_path);
		strcat(file_full_path, "\\");
		strcat(file_full_path, pm->texture_file_name);
		pm->texture=Load2DTexture(file_full_path);
	}
}

void MD5Model::ReleaseTextures(void)
{
	if (meshes==NULL) return;
	for (int i=0; i<num_meshes; ++i)
		glDeleteTextures(1, &meshes[i].texture);
}

void MD5Model::Draw(void)
{
	for (int i=0; i<num_meshes; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, meshes[i].texture);
		meshes[i].Draw();
	}
}

void MD5Model::DrawSkeleton(float joint_color[3], float bone_color[3])
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(MD5Joint), &current_skeleton[0].pos);

	// 将关节绘制成点
	glPointSize(5.0f);
	glColor3fv(joint_color);
	glDrawArrays(GL_POINTS, 0, num_joints);
	glPointSize(1.0f);

	// 将骨头绘制成线段
	unsigned long *bone_indices=new unsigned long [num_joints*2];
	int i, k=0;
	for (i=0; i<num_joints; ++i)
	{
		if (current_skeleton[i].parent!=-1)
		{
			bone_indices[k]=i;
			bone_indices[k+1]=current_skeleton[i].parent;
			k+=2;
		}
	}
	glLineWidth(3.0f);
	glColor3fv(bone_color);
	glDrawElements(GL_LINES, k, GL_UNSIGNED_INT, bone_indices);
	glLineWidth(1.0f);
	delete [] bone_indices;

	glDisableClientState(GL_VERTEX_ARRAY);
}