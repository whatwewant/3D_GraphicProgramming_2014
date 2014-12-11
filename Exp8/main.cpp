#include<windows.h>
#include"GL/glut.h"
#include"GL/glext.h"
#include"Mesh.h"
#include "ImageLib.h"
#include "Camera.h"
#include "ParticleSystem.h"
#include <mmsystem.h>

#pragma comment (lib, "winmm.lib")


bool rotation_mode = false;

CMesh ground_model ; // 地面模型
unsigned int ground_texture = 0; // 地面纹理

CCamera camera;
int mouse_x, mouse_y;
float camera_move_step=0.1f;
float mouse_sensitivity_x=1.0f;
float mouse_sensitivity_y=1.0f;

CParticleSystem fireworks; // 焰火

unsigned int current_time;

void GL_init(void) 
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

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

	camera.Init(CVector3D(15.0, 0.0, 1.0), 
		CVector3D(0.0, 0.0, 1.0), CVector3D(1.0, 0.0, 0.0));

	fireworks.Init(10000);
	fireworks.texture=Load2DTransparentTexture
		("../textures/particle01.jpg", "../textures/particle01a.jpg");

	current_time=timeGetTime();
}

void GL_cleanup(void) {
	glDeleteTextures(1, &ground_texture);
	glDeleteTextures(1, &fireworks.texture);
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

	// 绘制粒子
	fireworks.Draw(camera.u, camera.v);

	glFlush ();

	glutSwapBuffers();
}

void idle(void)
{
	unsigned int t=timeGetTime();
	unsigned int dt=t-current_time;
	
	if (dt<20) Sleep(10);
	else
	{
		float fdt=0.001f*(float)dt; // 以秒为单位的流逝时间
		current_time=t;
		float emit_rate=1000.0f;  // 单位时间内产生新粒子数量
		fireworks.Emit((int)(emit_rate*fdt)); // 产生新粒子
		fireworks.UpdateParticleStates(fdt);  // 更新现有粒子状态
		glutPostRedisplay();
	}
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
	glutIdleFunc(idle);

	glutMainLoop();

	GL_cleanup();
	return 0;
}