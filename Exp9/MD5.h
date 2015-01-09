#ifndef _MD5_MODEL_AND_ANIMATION_
#define _MD5_MODEL_AND_ANIMATION_

#include "Mesh.h"

// MD5顶点
struct MD5Vertex
{
	int start;  // 起始权重
	int count;  // 权重数量
};

// MD5权重
struct MD5Weight
{
	int   joint;    // 关节索引
	float bias;     // 混合因子
	CVector3D pos;  // 位置(关节局部坐标)
};

// MD5关节
struct MD5Joint
{
	int parent;         // 父关节索引，-1代表该关节是根关节
	CVector3D pos;      // 位置(平移)
	CQuaternion orient; // 方向(旋转)
};

// MD5网格类
class MD5Mesh: public CMesh
{
public:
	char texture_file_name[256]; // 纹理图像文件名
	unsigned int texture;        // 纹理对象索引

	int num_weights;       // 权重数量
	MD5Vertex *vertices2;  // 顶点额外信息数组
	MD5Weight *weights;    // 权重数组

	MD5Mesh(void);
	virtual void Release(void);
};

// MD5模型类
class MD5Model
{
public:
	int num_meshes;  // 网格数量
	int num_joints;  // 关节数量

	MD5Mesh *meshes;             // 网格数组
	MD5Joint *bindpose_skeleton; // 绑定姿势骨架
	MD5Joint *current_skeleton;  // 当前根据动画信息计算出的骨架

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

// MD5动画帧类
class MD5AnimationFrame
{
public:
	MD5Joint *skeleton; // 骨架
	CBoundingBox bbox;  // 包围盒

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
	int num_frames;    // 动画帧数量
	int num_joints;    // 关节数量

	int frame_rate;       // 每秒动画帧率
	float time_per_frame; // 每帧动画时间
	float total_time;     // 动画总时间

	MD5AnimationFrame *frames; // 动画帧数组

	MD5Animation (void);
	virtual ~MD5Animation (void) {Release();}
	virtual void Release(void);

	int Load(const char *file_name);
	void Animate(float time, MD5Joint *skeleton);

protected:
	void BuildFrameSkeleton(int num_components, 
		int *flag, int *start_index, float *frame_data,
		MD5Joint *base_skeleton, MD5Joint *frame_skeleton);
	// num_components: (输入) 动画帧压缩数据数量
	// flag: (输入) 关节的压缩方式标志
	// start_index: (输入) 关节在动画帧压缩数据中的起始位置索引
	// frame_data: (输入) 动画帧压缩数据
	// base_skeleton: (输入) 基准帧骨架
	// frame_skeleton: (输出) 计算出的动画帧骨架
};

#endif