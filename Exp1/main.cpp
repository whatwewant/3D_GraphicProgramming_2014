#include<windows.h>
#include"GL/glut.h"
#include"GL/glext.h"

void GL_init(void) 
{
	glClearColor (1.0, 1.0, 1.0, 0.0);
}

void GL_cleanup(void) {
	
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glTranslatef (0.0, 0.0, -3.0);

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
}

void keyboard (unsigned char key, int x, int y)
{
}

void mouse(int button, int state, int x, int y) 
{
}

void mouse_motion(int x, int y) 
{
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