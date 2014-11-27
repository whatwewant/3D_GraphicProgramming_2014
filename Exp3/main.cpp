#include<windows.h>
#include"GL/glut.h"
#include"GL/glext.h"
#include"ArcBall.h"
#include"Mesh.h"
#include "ImageLib.h"

CArcBall arc_ball;
CQuaternion previous_quat;
CQuaternion current_quat;
bool rotation_mode = false;

CMesh models[4];
int current_model_index = 0;
unsigned int model_textures[4]={0, 0, 0, 0}; // 三维模型纹理对象

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

	previous_quat.SetIdentity();
	current_quat.SetIdentity();

	/*
	models[0].CreateBlock(2.0, 1.6, 1.0);
	models[1].CreateSphere(1.0, 80, 80);
	models[2].CreateCylinder(0.5, 2.0, 40, 20);
	models[3].CreateTorus(0.2, 0.8, 40, 20);*/

	models[0].CreateBlock(2.0, 1.6, 1.0, 4.0, 2.0, 2.0);
	models[1].CreateSphere(1.0, 80, 80, 1.0, 1.0);
	models[2].CreateCylinder(0.5, 2.0, 40, 20, 3.0, 2.0, 1.0);
	models[3].CreateTorus(0.2, 0.8, 40, 20, 8.0, 2.0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	model_textures[0]=Load2DTexture("../textures/wood01.jpg");
	model_textures[1]=Load2DTexture("../textures/earth.jpg");
	model_textures[2]=Load2DTexture("../textures/metal01.jpg");
	model_textures[3]=Load2DTexture("../textures/glass01.jpg");

	glEnable(GL_TEXTURE_2D);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GL_cleanup(void) {
	glDeleteTextures(4, model_textures);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glTranslatef (0.0, 0.0, -3.0);

	// 设置光源位置
	GLfloat light_pos[4]={10.0, 8.0, 12.0, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	// 绘制模型
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 0.6, 0.6, 0.6, 1.0 };
	/*
	GLfloat mat_diffuse[] = { 1.0, 0.5, 0.0, 1.0 };
	GLfloat mat_specular[] = { 0.6, 0.6, 0.6, 1.0 };*/
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 64.0);

	CQuaternion q = current_quat * previous_quat;
	CMatrix3D rot_mat;
	q.GetRotationMatrix(rot_mat);
	glMultMatrixf ((float *)&rot_mat);

	glColor3f (0.0, 0.0, 0.0);
	// glutWireTeapot (1.0);
	// glutSolidTeapot (1.0);

	glBindTexture(GL_TEXTURE_2D, model_textures[current_model_index]);
	models[current_model_index].Draw();

	glFlush ();

	glutSwapBuffers();
}

void reshape (int w, int h) {
	glViewport (0, 0, w, h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (60.0, (float)w/h, 0.1, 6.0);
	arc_ball.SetSize(w, h);
}

bool use_wire_frame = false;
bool use_smooth_shading = true;
void keyboard (unsigned char key, int x, int y)
{
	if (key == 27)
	{
		GL_cleanup();
		exit(0);
	} 
	else if (key == '1')
	{
			current_model_index = 0;
			glutPostRedisplay();
	}
	else if (key == '2')
	{
			current_model_index = 1;
			glutPostRedisplay();
	}
	else if (key == '3')
	{
			current_model_index = 2;
			glutPostRedisplay();
	}
	else if (key == '4')
	{
			current_model_index = 3;
			glutPostRedisplay();
	}
	else if (key=='w' || key=='W')
	{
		use_wire_frame=!use_wire_frame;
		glPolygonMode(GL_FRONT_AND_BACK, use_wire_frame?GL_LINE:GL_FILL);
		glutPostRedisplay();
	}
	else if (key=='s' || key=='S')
	{
		use_smooth_shading=!use_smooth_shading;
		glShadeModel(use_smooth_shading?GL_SMOOTH:GL_FLAT);
		glutPostRedisplay();
	}

}

void mouse(int button, int state, int x, int y) 
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN) 
		{
			rotation_mode = true;
			arc_ball.SetStartPoint(x, y);
		} else {
			rotation_mode = false;
			previous_quat = current_quat * previous_quat;
			current_quat.SetIdentity();
		}
	}
}

void mouse_motion(int x, int y) 
{
	if (rotation_mode) {
		arc_ball.SetEndPoint(x, y);
		arc_ball.GetQuaternion(current_quat);
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

	glutMainLoop();

	GL_cleanup();
	return 0;
}