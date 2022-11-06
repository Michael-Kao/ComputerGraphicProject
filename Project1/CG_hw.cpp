#include <bits/stdc++.h>
#include <GL/glut.h>

using namespace std;

float red = 1.0f, blue = 1.0f, green = 1.0f; //所有變數值設為1表示圖形最初為白色

float angle = 0.0f; // angle of rotation for the camera direction
float lx = 0.0f, lz = -1.0f; // actual vector representing the camera's direction
float x = 0.0f, z = 5.0f; //XZ position of the camera

void drawSnowMan()
{
	glColor3f(1.0f, 1.0f, 1.0f); // 所有值都為1.0表示圖形為白色
	//Draw body
	glTranslatef(0.0f, 0.75f, 0.0f);
	glutSolidSphere(0.75f, 20, 20);
	//Draw head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.25f, 20, 20);
	//Draw Eyes
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.10f, 0.18f);
	glutSolidSphere(0.05f, 10, 10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f, 10, 10);
	glPopMatrix();
	//Draw Nose
	glColor3f(1.0f, 0.5f, 0.5f);
	glutSolidCone(0.08, 0.5f, 10, 2);
}

void renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(	x, 1.0f, z, //camera position
				x + lx, 1.0f,  z + lz, //the point we are looking at,  Look At Point = Line Of Sight + Camera Position
				0.0f, 1.0f,  0.0f  //the up vector, usually set to (0.0, 1.0, 0.0), meaning that camera's is not tilted. upside down can use (0.0, -1.0, 0.0)
	); //設定相機
	//glRotatef(angle, 0.0f, 1.0f, 0.0f);
	// Draw ground
	glColor3f(0.9f, 0.9f, 0.9f); // responsible for setting color
	glBegin(GL_QUADS);
			glVertex3f(-100.0f, 0.0f, -100.0f);//相機預設照往z方向的負值，所以z軸值為負才能出現在畫面上
			glVertex3f(-100.0f, 0.0f,  100.0f);
			glVertex3f( 100.0f, 0.0f,  100.0f);
			glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();
	// Draw 36 snowmen
	for (int i = -3; i < 3; i++)
	{
		for (int j = -3; j < 3; j++)
		{
			glPushMatrix();
			glTranslatef(i * 10.0f, 0, j * 10.0f);
			drawSnowMan();
			glPopMatrix();
		}
	}
	//angle += 0.1f;
	glutSwapBuffers();

}

void changeSize(int w, int h)
{
	//prevent a divide by zero, when window is too short
	//you can't make a window of zero width
	if (h == 0)
		h = 1;
	float ratio = 1.0 * w / h;
	
	//use the projection matrix
	glMatrixMode(GL_PROJECTION);
	// reset matrix
	glLoadIdentity();
	//set the viewport to be the entire window
	glViewport(0, 0, w, h);
	//set the correct perspective
	gluPerspective(45, ratio, 1, 1000); //
	//get back to the modelview
	glMatrixMode(GL_MODELVIEW);
}

void processNormalKeys(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
}

void processSpecialKeys(int key, int x, int y)
{
	int mod;
	float fraction = 0.1f;
	switch (key)
	{
		case GLUT_KEY_F1 : 
			mod = glutGetModifiers();
			if (mod == (GLUT_ACTIVE_CTRL | GLUT_ACTIVE_ALT))
			{
				red = 1.0;
				green = 0.0;
				blue = 0.0;
			}
			break;
		case GLUT_KEY_F2 :
			red = 0.0;
			green = 1.0;
			blue = 0.0;
			break;
		case GLUT_KEY_F3 :
			red = 0.0;
			green = 0.0;
			blue = 1.0;
			break;
		case GLUT_KEY_LEFT :
			angle -= 0.01f;
			lx = sin(angle);
			lz = -cos(angle);
			break;
		case GLUT_KEY_RIGHT :
			angle += 0.01f;
			lx = sin(angle);
			lz = -cos(angle);
			break;
		case GLUT_KEY_UP :
			x += lx * fraction;
			z += lz * fraction;
			break;
		case GLUT_KEY_DOWN :
			x -= lx * fraction;
			z -= lz * fraction;
			break;
	}
}


int main(int argc, char** argv)
{
	//init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("My_CGTest");
	//register callbacks
	glutDisplayFunc(renderScene);// register callbacks
	glutReshapeFunc(changeSize);//
	glutIdleFunc(renderScene);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();//GLUT event processing cycle
	return 1;
}