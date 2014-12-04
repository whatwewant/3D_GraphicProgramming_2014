#include<windows.h>
#include"GL/glut.h"
#include"GL/glext.h"
#include"Mesh.h"
#include "ImageLib.h"
#include "Camera.h"
#include "Skybox.h"

#include "WaterSurface.h"
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")


bool rotation_mode = false;

CMesh models[4];
unsigned int model_textures[4]={0, 0, 0, 0}; // 三维模型纹理对象

CMesh ground_model ; // 地面模型
unsigned int ground_texture = 0; // 地面纹理

CCamera camera;
int mouse_x, mouse_y;
float camera_move_step=0.2f;
float mouse_sensitivity_x=1.0f;
float mouse_sensitivity_y=1.0f;

CSkybox sky_box; // 天空盒
unsigned int env_texture; // 环境图(立方图纹理)

void DrawSkybox(void)
{
	// 关闭深度测试
	glDisable(GL_DEPTH_TEST);

	// 关闭光照
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);

	// 保存投影变换矩阵
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// 增大透视投影变换远平面距离
	glLoadIdentity();
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluPerspective(60.0, (float)viewport[2]/(float)viewport[3], 
		sky_box.size*0.01, sky_box.size*3.0);

	// 绘制天空盒
	sky_box.Draw();

	// 恢复投影变换矩阵
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// 重新打开光照
	glEnable(GL_LIGHTING);

	// 重新打开深度测试
	glEnable(GL_DEPTH_TEST);
}

CWaterSurface water_surface;
float water_surface_height=-2.0f;

unsigned int current_time;

void GL_init(void) 
{
	glClearColor (1.0, 1.0, 1.0, 0.0);

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

	models[0].CreateBlock(2.0, 1.6, 1.0, 4.0, 2.0, 2.0);
	models[1].CreateSphere(1.0, 80, 80, 1.0, 1.0);
	models[2].CreateCylinder(0.5, 2.0, 40, 20, 3.0, 2.0, 1.0);
	models[3].CreateTorus(0.2, 0.8, 40, 20, 8.0, 2.0);
	ground_model.CreateTerrainFromHeightMap(140.0, 140.0, 32.0, 32.0,
		"../textures/terrain_height-01.jpg", -21.0f, 0.3f);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	model_textures[0]=Load2DTexture("../textures/wood01.jpg");
	model_textures[1]=Load2DTexture("../textures/earth.jpg");
	model_textures[2]=Load2DTexture("../textures/metal01.jpg");
	model_textures[3]=Load2DTexture("../textures/glass01.jpg");
	ground_texture=Load2DMipmapTexture("../textures/rock01.jpg");

	glEnable(GL_TEXTURE_2D);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	camera.Init(CVector3D(15.0, 0.0, 1.0), 
		CVector3D(0.0, 0.0, 1.0), CVector3D(1.0, 0.0, 0.0));

	// Sky Box
	sky_box.size=1000.0f;
	sky_box.Create();
	env_texture=LoadCubemapTexture(
		"../textures/Skybox01_left.jpg",
		"../textures/Skybox01_right.jpg",
		"../textures/Skybox01_up.jpg",
		"../textures/Skybox01_down.jpg",
		"../textures/Skybox01_front.jpg",
		"../textures/Skybox01_back.jpg"
	);

	water_surface.Create(140.0, 140.0, 64, 64);
	water_surface.plane_waves[0].A=0.4f;
	water_surface.plane_waves[0].f=0.5f;
	water_surface.plane_waves[0].Lx_rcp=4.0f/100.0f;
	water_surface.plane_waves[0].Ly_rcp=0.0f/100.0f;
	water_surface.plane_waves[1].A=0.1f;
	water_surface.plane_waves[1].f=1.0f;
	water_surface.plane_waves[1].Lx_rcp=10.0f/100.0f;
	water_surface.plane_waves[1].Ly_rcp=10.0f/100.0f;

	current_time=timeGetTime();
}

void GL_cleanup(void) {
	glDeleteTextures(4, model_textures);
	glDeleteTextures(1, &ground_texture);
	glDeleteTextures(1, &env_texture);
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
	GLfloat light_pos[4]={10.0, 8.0, 12.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	// 绘制天空盒 2
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// 绘制天空盒 1 
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, env_texture);
	DrawSkybox();
	glDisable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_2D);
	// 绘制天空盒 2
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);


	// 绘制模型
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 0.0);

	glBindTexture(GL_TEXTURE_2D, ground_texture);
	ground_model.Draw();

	// 绘制其它模型
	GLfloat mat_specular2[] = { 0.6, 0.6, 0.6, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
	glMaterialf(GL_FRONT, GL_SHININESS, 64.0);

	glBindTexture(GL_TEXTURE_2D, model_textures[0]);
	glPushMatrix();
	glTranslatef(-4.0f, -4.0f, 0.5f);
	models[0].Draw();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, model_textures[1]);
	glPushMatrix();
	glTranslatef(4.0f, -4.0f, 1.0f);
	models[1].Draw();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, model_textures[2]);
	glPushMatrix();
	glTranslatef(4.0f, 4.0f, 1.0f);
	models[2].Draw();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, model_textures[3]);
	glPushMatrix();
	glTranslatef(-4.0f, 4.0f, 1.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	models[3].Draw();
	glPopMatrix();

	// 绘制水面
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLfloat water_diffuse[] = { 0.7, 0.7, 1.0, 0.8 };
	GLfloat water_specular[] = { 0.0, 0.0, 0.0, 0.8 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, water_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, water_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 64.0);

	glPushMatrix();
	glTranslatef(0.0, 0.0, water_surface_height);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(-90.0, 1.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, env_texture);
	water_surface.Draw();
	glDisable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

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
		current_time=t;
		water_surface.Animate(0.001*t);
		CVector3D camera_pos=camera.P0;
		camera_pos.z-=water_surface_height;
		water_surface.CalculateReflectionVectors(camera_pos);
		glutPostRedisplay();
	}
}

void reshape (int w, int h) {
	glViewport (0, 0, w, h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w/(float)h, 0.1, 200.0);
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