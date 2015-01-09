#include <windows.h>
#include "GL/glut.h"
#include "GL/glext.h"
#include "Mesh.h"
#include "ImageLib.h"
#include "Camera.h"
#include "MD5.h"

CCamera camera;
int mouse_x, mouse_y;
float camera_move_step=0.1f;
float mouse_sensitivity_x=1.0f;
float mouse_sensitivity_y=1.0f;

bool rotation_mode=false;

CMesh ground_model; // 地面模型
unsigned int ground_texture=0; // 地面纹理

LARGE_INTEGER current_time;
double high_res_period; // 高分辨率计时器周期

MD5Model     g_md5_model;   // MD5模型
MD5Animation g_md5_anim;    // MD5动画
bool model_loaded=false;    // 模型是否载入
int model_draw_state=1;     // 模型绘制模式 (0---隐藏, 1---实心, 2---线框)
int show_skeleton=0;        // 是否显示骨架
int pause_animation=0;      // 动画是否暂停
float animation_time=0.0f;  // 动画时间
float viewport_aspect=1.0f; // 视口宽高比
float scene_size=1.0f;      // 场景尺寸
CVector3D ground_center;    // 地面中心

int OpenFileDialog(char *file_path_buffer, int buffer_size,
	const char *filter, const char *dialog_title)
{
	file_path_buffer[0]=0;

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = file_path_buffer;
	ofn.nMaxFile = buffer_size;
	ofn.lpstrFilter = filter;
	ofn.lpstrTitle = dialog_title;

	if( GetOpenFileName(&ofn) ) return 1;
	else return 0;
}

void CloseModelAndAnimationFiles(void)
{
	g_md5_model.ReleaseTextures();
	g_md5_model.Release();
	g_md5_anim.Release();
	model_loaded=false;
}

void LoadModelAndAnimationFiles(void)
{
	if (model_loaded) CloseModelAndAnimationFiles();

	char model_file_path[1024], anim_file_path[1024];

	// 获取模型文件名
	if (!OpenFileDialog(model_file_path, sizeof(model_file_path),
		"MD5模型文件(*.md5mesh)\0*.md5mesh\0所有文件\0*.*\0\0",
		"打开MD5模型文件")) return;

	// 获取动画文件名
	if (!OpenFileDialog(anim_file_path, sizeof(anim_file_path),
		"MD5动画文件(*.md5anim)\0*.md5anim\0所有文件\0*.*\0\0",
		"打开MD5动画文件")) return;

	// 载入模型文件
	if (!g_md5_model.Load(model_file_path)) 
	{
		g_md5_model.Release();
		return;
	}

	// 载入动画文件
	if (!g_md5_anim.Load(anim_file_path)) 
	{
		g_md5_model.Release();
		g_md5_anim.Release();
		return;
	}

	// 合并动画帧包围盒
	CBoundingBox bbox=g_md5_anim.frames[0].bbox;
	int i;
	for (i=1; i<g_md5_anim.num_frames; ++i)
		bbox+=g_md5_anim.frames[i].bbox;

	// 设置照相机参数
	CVector3D bbox_center=bbox.GetCenter();
	CVector3D bbox_size=bbox.GetSize();
	scene_size=bbox_size.x;
	if (scene_size<bbox_size.y) scene_size=bbox_size.y;
	if (scene_size<bbox_size.z) scene_size=bbox_size.z;
	CVector3D camera_center=bbox_center;
	camera_center.y+=scene_size;
	camera.Init(camera_center, CVector3D(0.0, 0.0, 1.0), CVector3D(0.0, 1.0, 0.0));
	scene_size*=2.0f;
	camera_move_step=0.02f*scene_size;

	// 设置透视投影变换(修改远平面距离以适应模型大小)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, viewport_aspect, 0.01f*scene_size, 3.0f*scene_size);

	// 修改地面尺寸以适应模型大小
	ground_model.Release();
	ground_model.CreateRectangle(scene_size, scene_size, 32, 32, 16.0, 16.0);
	ground_center=bbox_center;
	ground_center.z=bbox.min.z;

	// 初始化模型纹理
	char *str=strrchr(model_file_path, '\\');
	if (str==NULL) strcpy(model_file_path, "../textures");
	else *str=0;
	g_md5_model.InitTextures(model_file_path);

	model_loaded=true;
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

	// 获取高分辨率计时器频率
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	// 计算高分辨率计时器周期
	high_res_period=1.0/(double)freq.QuadPart;

	QueryPerformanceCounter(&current_time);
}

void GL_cleanup(void)
{
	glDeleteTextures(1, &ground_texture);
	if (model_loaded) CloseModelAndAnimationFiles();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!model_loaded)
	{
		glFlush();
		glutSwapBuffers();
		return;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 设置视图变换
	CMatrix3D view_mat;
	camera.GetViewMatrix(view_mat);
	glMultMatrixf((GLfloat *)&view_mat);

	// 设置光源位置
	GLfloat light_pos[4]={10.0, 8.0, 12.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	// 绘制地面
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 0.6, 0.6, 0.6, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 64.0);

	glPushMatrix();
	glTranslatef(ground_center.x, ground_center.y, ground_center.z);
    glBindTexture(GL_TEXTURE_2D, ground_texture);
	ground_model.Draw();
	glPopMatrix();

	// 绘制模型
	if (show_skeleton)
	{
		float joint_color[3]={1.0, 0.0, 0.0};
		float bone_color[3]={0.0, 0.0, 1.0};
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		g_md5_model.DrawSkeleton(joint_color, bone_color);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}

	if (model_draw_state==2)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_LINE);
		glColor3f(0.0, 0.0, 0.0);
		g_md5_model.Draw();
		glPolygonMode(GL_FRONT, GL_FILL);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}
	else if (model_draw_state==1)
		g_md5_model.Draw();

	glFlush();

	glutSwapBuffers();
}

void reshape(int w,int h)
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	viewport_aspect=(float)w/(float)h;
	gluPerspective(60.0, viewport_aspect, 0.01f*scene_size, 3.0f*scene_size);
}

void keyboard(unsigned char key,int x,int y)
{
	switch (key) 
	{
		case 27:
			GL_cleanup();
			exit(0); break;
		case '1':
			++model_draw_state;
			if (model_draw_state>=3) model_draw_state=0;
			glutPostRedisplay(); break;
		case '2':
			show_skeleton=1-show_skeleton;
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
		case 'l':
		case 'L':
			LoadModelAndAnimationFiles();
			glutPostRedisplay();
			break;
		case 'c':
		case 'C':
			CloseModelAndAnimationFiles();
			glutPostRedisplay();
			break;
		case ' ':
			pause_animation=1-pause_animation;
			break;
	}
}

void mouse(int button,int state,int x,int y)
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

void mouse_motion(int x,int y)
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

void idle(void)
{
	// 检查模型是否载入
	if (!model_loaded)
	{
		Sleep(10);
		return;
	}

	LARGE_INTEGER t, dt;
	double ddt;

	// 获取当前时间
	QueryPerformanceCounter(&t);

	// 处理画面更新
	dt.QuadPart=t.QuadPart-current_time.QuadPart;
	ddt=high_res_period*(double)dt.QuadPart;
	if (ddt<0.02) Sleep(10);
	else
	{
		current_time=t;
		if (!pause_animation)
		{
			animation_time+=(float)ddt;
			if (animation_time>g_md5_anim.total_time)
				animation_time=0.0f;
			g_md5_anim.Animate(animation_time, g_md5_model.current_skeleton);
			g_md5_model.CalculateVertexPositions();
			g_md5_model.CalculateVertexAverageNormals();
			glutPostRedisplay();
		}
	}
}

int main(int argc,char * argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutInitWindowPosition(100,50);
	glutCreateWindow("OpenGL Program");

	GL_init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_motion);
	glutIdleFunc(idle);

	glutMainLoop();

	GL_cleanup();

	return 0;

}