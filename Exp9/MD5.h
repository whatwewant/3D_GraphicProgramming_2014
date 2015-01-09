#ifndef _MD5_MODEL_AND_ANIMATION_
#define _MD5_MODEL_AND_ANIMATION_

#include "Mesh.h"

// MD5����
struct MD5Vertex
{
	int start;  // ��ʼȨ��
	int count;  // Ȩ������
};

// MD5Ȩ��
struct MD5Weight
{
	int   joint;    // �ؽ�����
	float bias;     // �������
	CVector3D pos;  // λ��(�ؽھֲ�����)
};

// MD5�ؽ�
struct MD5Joint
{
	int parent;         // ���ؽ�������-1����ùؽ��Ǹ��ؽ�
	CVector3D pos;      // λ��(ƽ��)
	CQuaternion orient; // ����(��ת)
};

// MD5������
class MD5Mesh: public CMesh
{
public:
	char texture_file_name[256]; // ����ͼ���ļ���
	unsigned int texture;        // �����������

	int num_weights;       // Ȩ������
	MD5Vertex *vertices2;  // ���������Ϣ����
	MD5Weight *weights;    // Ȩ������

	MD5Mesh(void);
	virtual void Release(void);
};

// MD5ģ����
class MD5Model
{
public:
	int num_meshes;  // ��������
	int num_joints;  // �ؽ�����

	MD5Mesh *meshes;             // ��������
	MD5Joint *bindpose_skeleton; // �����ƹǼ�
	MD5Joint *current_skeleton;  // ��ǰ���ݶ�����Ϣ������ĹǼ�

	MD5Model(void);
	virtual ~MD5Model(void) {Release();}
	virtual void Release(void);

	int Load(const char *file_name);
	void CalculateVertexPositions(void);
	void CalculateVertexAverageNormals(void);
	void InitTextures(const char *texture_path);
	void ReleaseTextures(void);
	void Draw(void);
	void DrawSkeleton(float joint_color[3], float bone_color[3]);
};

// MD5����֡��
class MD5AnimationFrame
{
public:
	MD5Joint *skeleton; // �Ǽ�
	CBoundingBox bbox;  // ��Χ��

	MD5AnimationFrame(void) {skeleton=NULL;}
	virtual ~MD5AnimationFrame(void)
	{
		if (skeleton!=NULL) delete [] skeleton; 
		skeleton=NULL;
	}
};

// MD5 animation class
class MD5Animation
{
public:
	int num_frames;    // ����֡����
	int num_joints;    // �ؽ�����

	int frame_rate;       // ÿ�붯��֡��
	float time_per_frame; // ÿ֡����ʱ��
	float total_time;     // ������ʱ��

	MD5AnimationFrame *frames; // ����֡����

	MD5Animation (void);
	virtual ~MD5Animation (void) {Release();}
	virtual void Release(void);

	int Load(const char *file_name);
	void Animate(float time, MD5Joint *skeleton);

protected:
	void BuildFrameSkeleton(int num_components, 
		int *flag, int *start_index, float *frame_data,
		MD5Joint *base_skeleton, MD5Joint *frame_skeleton);
	// num_components: (����) ����֡ѹ����������
	// flag: (����) �ؽڵ�ѹ����ʽ��־
	// start_index: (����) �ؽ��ڶ���֡ѹ�������е���ʼλ������
	// frame_data: (����) ����֡ѹ������
	// base_skeleton: (����) ��׼֡�Ǽ�
	// frame_skeleton: (���) ������Ķ���֡�Ǽ�
};

#endif