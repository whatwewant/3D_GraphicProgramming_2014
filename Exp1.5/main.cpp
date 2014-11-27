#include<windows.h>
#include"GL/glut.h"
#include"GL/glext.h"
#include"ArcBall.h"

CArcBall arc_ball;
CQuaternion previous_quat;
CQuaternion current_quat;
bool rotation_mode = false;

void GL_init(void) 
{
	glClearColor (1.0, 1.0, 1.0, 0.0);

	previous_quat.SetIdentity();
	current_quat.SetIdentity();
}

void GL_cleanup(void) {
	
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glTranslatef (0.0, 0.0, -3.0);

	CQuaternion q = current_quat * previous_quat;
	CMatrix3D rot_mat;
	q.GetRotationMatrix(rot_mat);
	glMultMatrixf ((float *)&rot_mat);

	glColor3f (0.0, 0.0, 0.0);
	glutWireTeapot (1.0);
	// glutSolidTeapot (1.0);

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

void keyboard (unsigned char key, int x, int y)
{
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