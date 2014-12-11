#ifndef _PARTICLE_SYSTEM_
#define _PARTICLE_SYSTEM_

#include "MathLib.h"

// 粒子类
class CParticle
{
public:
	CVector3D x;     // 位置
	CVector3D v;     // 速度
	float size;      // 大小
	float lifespan;  // 寿命
	float age;       // 年龄
	unsigned char active; // 激活标志
};

// 粒子四边形顶点类
class CParticleQuadVertex
{
public:
	CVector3D position; // 位置
	float texcoord[2];  // 纹理坐标
};

// 粒子系统类
class CParticleSystem
{
protected:
	int num_particles;         // 粒子总数量
	
	CParticle *particles;  // 粒子数组
	CParticleQuadVertex *quad_vertices; // 粒子四边形顶点数组

public:
	float color[4];       // 颜色
	unsigned int texture; // 纹理
	float gravity;        // 重力加速度

	CParticleSystem(void);
	~CParticleSystem(void) {Release();}

	void Init(int _num_particles);
	void Release(void);

	void Emit(int n);
	void UpdateParticleStates(float dt);
	void Draw(const CVector3D& camera_right_dir, const CVector3D& camera_up_dir);
};

#endif
