#include<windows.h>
#include"GL/glut.h"
#include"GL/glext.h"
#include"Mesh.h"
#include "ImageLib.h"
#include "Camera.h"
#include "Cloth.h"

bool rotation_mode = false;

CMesh ground_model ; // 地面模型
unsigned int ground_texture = 0; // 地面纹理

CCamera camera;
int mouse_x, mouse_y;
float camera_move_step=0.1f;
float mouse_sensitivity_x=1.0f;
float mouse_sensitivity_y=1.0f;

float sphere_radius=0.5f;
CVector3D sphere_center=CVector3D(0.0f, 0.2f*sphere_radius, 1.5f*sphere_radius);
CMesh sphere_model;
unsigned int sphere_texture=0;

CCloth the_cloth; // 布料

float gravity=9.8f;     // 重力加速度
float kfriction=0.2f;   // 地面与布料之间的磨擦系数
float time_step=0.002f; // 时间积分步长

LARGE_INTEGER current_time;
LARGE_INTEGER current_time2;
double high_res_period; // 高分辨率计时器周期

void InitCloth(void)
{
	unsigned long nx=32, ny=32, i;

	// 释放布料占据的存储空间
	the_cloth.Release();

	// 初始化布料
	the_cloth.Create(1.6f, 2.0f, nx, ny, 4, 4,
		CVector3D(0.0f, 0.0f, sphere_center.z+sphere_radius*2.0f));

	// 固定布料一端
	for (i=0; i<=nx; ++i) the_cloth.particles[i].fixed=1;
}

void GL_init(void) 
{
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	static GLfloat light_ambient[] = { 0.01, 0.01, 0.01, 1.0 };
	static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	ground_model.CreateRectangle(32.0, 32.0, 32, 32, 16.0, 16.0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	ground_texture=Load2DMipmapTexture("../textures/marble01.jpg");

	glEnable(GL_TEXTURE_2D);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	camera.Init(CVector3D(sphere_radius*5.0f, 0.0, sphere_radius*4.0f), 
		CVector3D(0.0, 0.0, 1.0), CVector3D(1.0, 0.0, 0.0));
	camera.LookUp(-20.0f);

	sphere_texture=Load2DTexture("../textures/glass01.jpg");
	the_cloth.texture=Load2DMipmapTexture("../textures/cloth01.jpg");

	sphere_model.CreateSphere(sphere_radius, 40, 40, 6.0f, 3.0f);
	InitCloth();

	// 获取高分辨率计时器频率
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	// 计算高分辨率计时器周期
	high_res_period=1.0/(double)freq.QuadPart;

	QueryPerformanceCounter(&current_time);
	QueryPerformanceCounter(&current_time2);
}

void GL_cleanup(void) {
	glDeleteTextures(1, &ground_texture);
	glDeleteTextures(1, &the_cloth.texture);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	// 设置视图变换
	CMatrix3D view_mat;
	camera.GetViewMatrix(view_mat);
	glMultMatrixf((GLfloat *)&view_mat);
	


	// 设置光源位置
	GLfloat light_pos[4]={10.0, 8.0, 12.0, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	// 绘制模型
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 0.6, 0.6, 0.6, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 64.0);

	// 绘制地面
	glBindTexture(GL_TEXTURE_2D, ground_texture);
	ground_model.Draw();

	// 绘制球体
	glBindTexture(GL_TEXTURE_2D, sphere_texture);
	glPushMatrix();
	glTranslatef(sphere_center.x, sphere_center.y, sphere_center.z);
	sphere_model.Draw();
	glPopMatrix();

	// 绘制布料
	float mat_specular2[4]={0.0, 0.0, 0.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular2);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0);

	glBindTexture(GL_TEXTURE_2D, the_cloth.texture);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	glDisable(GL_CULL_FACE);
	the_cloth.Draw();
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	glEnable(GL_CULL_FACE);



	glFlush ();

	glutSwapBuffers();
}

void reshape (int w, int h) {
	glViewport (0, 0, w, h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w/(float)h, 0.1, 60.0);
}

bool use_wire_frame = false;
bool use_smooth_shading = true;
void keyboard (unsigned char key, int x, int y)
{
	switch (key) 
	{
		case 27:
			GL_cleanup();
			exit(0); break;
		case '1':
			use_wire_frame=!use_wire_frame;
			glPolygonMode(GL_FRONT_AND_BACK, use_wire_frame?GL_LINE:GL_FILL);
			glutPostRedisplay(); break;
		case '2':
			use_smooth_shading=!use_smooth_shading;
			glShadeModel(use_smooth_shading?GL_SMOOTH:GL_FLAT);
			glutPostRedisplay(); break;
		case 'w':
		case 'W':
			camera.MoveForward(camera_move_step);
			glutPostRedisplay(); break;
		case 's':
		case 'S':
			camera.MoveForward(-camera_move_step);
			glutPostRedisplay(); break;
		case 'a':
		case 'A':
			camera.MoveRight(-camera_move_step);
			glutPostRedisplay(); break;
		case 'd':
		case 'D':
			camera.MoveRight(camera_move_step);
			glutPostRedisplay(); break;
		case 'r':
		case 'R':
			camera.MoveUp(camera_move_step);
			glutPostRedisplay(); break;
		case 'f':
		case 'F':
			camera.MoveUp(-camera_move_step);
			glutPostRedisplay(); break;
		case 'z':
		case 'Z':
			InitCloth();
			glutPostRedisplay(); break;

	}

}

void idle(void)
{
	LARGE_INTEGER t, dt;
	double ddt;

	// 获取当前时间
	QueryPerformanceCounter(&t);

	// 处理画面更新
	dt.QuadPart=t.QuadPart-current_time.QuadPart;
	ddt=high_res_period*(double)dt.QuadPart;
	if (ddt>0.02)
	{
		current_time=t;
		the_cloth.CalculateVertexAverageNormals();
		glutPostRedisplay();
	}

	// 处理布料动画
	dt.QuadPart=t.QuadPart-current_time2.QuadPart;
	ddt=high_res_period*(double)dt.QuadPart;
	if (ddt>time_step)
	{
		current_time2=t;

		the_cloth.ResetForces();
		the_cloth.ApplyGravity(gravity);
		the_cloth.ApplySpringForces();

		the_cloth.CollisionWithSphere(sphere_center, sphere_radius);
		the_cloth.CollisionWithGround(0.0f, kfriction, gravity);

		the_cloth.UpdateParticleStates(time_step);
	}
}

void mouse(int button, int state, int x, int y) 
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			rotation_mode=true;
			mouse_x=x; mouse_y=y;
		}
		else
			rotation_mode=false;
	}

}

void mouse_motion(int x, int y) 
{
	if (rotation_mode)
	{
		float ax=mouse_sensitivity_x*(x-mouse_x);
		camera.TurnLeft(-ax);
		float ay=mouse_sensitivity_y*(y-mouse_y);
		camera.LookUp(-ay);
		mouse_x=x; mouse_y=y;
		glutPostRedisplay();
	}

}

int main(int argc, char * argv[]) {	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 600);
	glutInitWindowPosition (100, 50);
	glutCreateWindow("OpenGLApp");

	GL_init();

	glutDisplayFunc (display);
	glutReshapeFunc (reshape);
	glutKeyboardFunc (keyboard);
	glutMouseFunc (mouse);
	glutMotionFunc (mouse_motion);
	glutIdleFunc (idle);

	glutMainLoop();

	GL_cleanup();
	return 0;
}