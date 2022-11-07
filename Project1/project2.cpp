/*---------------------------------------------------------------------------------
 * This file contains a program, drawing & animating a wind-mill.
 * This program is used to teach LCS and scene graph concepts.
 *
 * by S. K. Ueng, Nov.  2006
 */
#include <bits/stdc++.h>
#include <GL/glut.h>

using namespace std;

enum MODE {
    STOP, WALK, JUMP, GESTURE
};

enum WEAPON {
    NONE, SWORD, LANCE
};

WEAPON weapon = NONE;
MODE mode = STOP;

/*-----Define a unit box--------*/
/* Vertices of the box */
float  points[][3] = { {0.0, 0.0, 0.0}, {1.5, 0.0, 0.0},
                      {1.5, 1.5, 0.0}, {0.0, 1.5, 0.0},
                      {0.0, 0.0, 2.5}, {1.5, 0.0, 2.5},
                      {1.5, 1.5, 2.5}, {0.0, 1.5, 2.5} };
/* face of box, each face composing of 4 vertices */
int    face[][4] = { {0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5},
                    {4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3} };
/* indices of the box faces */
int    cube[6] = { 0, 1, 2, 3, 4, 5 };

/*-----Define GLU quadric objects, a sphere and a cylinder----*/
GLUquadricObj* sphere = NULL, * cylind = NULL;

/*-----Define window size----*/
int width = 512, height = 512;

//Global variables recording rotation angles of H-arm and blades.
float  arm_ang = 45.0, bld_ang = 10.0;

double Pi = 3.1415926;
float view_x = 1.0, view_y = 1.0, view_z = 1.0;
float test_x = 0.0, test_y = 0.0, test_z = 0.0;
int ges_fore = 0, gesture = 0, spin = 0, ang_x = 0, ang_y = 0, ang_z = 0, r_x = 0, r_y = 0, r_z = 0, Lfa_ang = 0, Rfa_ang = 0, La_ang = 0, Ra_ang = 0, r_calf = 0, l_calf = 0, r_leg = 0, l_leg = 0, angle_speed = 1, counter = 0, turn_ang = 0;
bool ges = false, D = false, one_side_move = false, jump = false, stop = true, crouch = false, run = false, hold = false, right_hand = true;
float run_speed = 1.0;
float crouch_down = 0.0, calf_down = 0.0, leg_down = 0.0; //no use
int jump_s = 0;

void draw_sword();
void draw_lance();

/*----------------------------------------------------------
 * Procedure to initialize the working environment.
 */
void  myinit()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);      /*set the background color BLACK */
    /*Clear the Depth & Color Buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, 499, 499);

    /*-----Set a parallel projection mode-----*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-40.0, 40.0, -40, 40, 0.0, 120.0);

    glEnable(GL_DEPTH_TEST);  /*Enable depth buffer for shading computing */

    glFlush();/*Enforce window system display the results*/
}


/*-------------------------------------------------------
 * Procedure to draw a polygon as a blade of the windmill
 */
void draw_blade()
{
    glBegin(GL_POLYGON);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 4.0, 0.0);
    glVertex3f(1.0, 8.0, 0.0);
    glVertex3f(-1.0, 8.0, 0.0);
    glVertex3f(-1.0, 4.0, 0.0);
    glEnd();
}

/*--------------------------------------------------------
 * Procedure to draw a cube. The geometrical data of the cube
 * are defined above.
 */
void draw_ret()
{
    int    i;

    glColor3f(1.0, 0.0, 1.0);  /* Set the current color */
    for (i = 0; i < 6; i++) {
        glBegin(GL_POLYGON);  /* Draw the face */
        glVertex3fv(points[face[i][0]]);
        glVertex3fv(points[face[i][1]]);
        glVertex3fv(points[face[i][2]]);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
}


/*------------------------------------------------------------
 * Procedure to draw a floor of black-and-white tiles.
  */
void draw_floor()
{
    int   i, j;

    for (i = -60; i < 60; i++)
        for (j = -60; j < 60; j++) {
            //if ((i + j) % 2 == 0) glColor3f(0.9, 0.9, 0.9);
            /*else*/ glColor3f(0.3, 0.1, 0.1);
            glBegin(GL_POLYGON);
            glVertex3f(i, 0.0, j);
            glVertex3f(i, 0.0, j + 1);
            glVertex3f(i + 1, 0.0, j + 1);
            glVertex3f(i + 1, 0.0, j);
            glEnd();
        }
}

//change degree to radian
double DEG2RAD(int deg) {
    double x;
    x = deg * Pi / 180;
    return x;
}

//caculate how height we reduce by crouching //not using now
void caldown_func()
{
    float leg = 2.5, calf = 6.0;
    double l_deg, c_deg, tmp_l, tmp_c;
    l_deg = DEG2RAD(r_leg * (-1));
    c_deg = DEG2RAD(r_calf);
    tmp_l = leg * sin(l_deg);
    tmp_c = calf * sin(c_deg);
    cout << "sin_leg = " << sin(l_deg) << " sin_calf = " << sin(c_deg) << endl;
    if (tmp_l > -1 && tmp_c > -1) {
        leg_down = leg - tmp_l;
        calf_down = calf - tmp_c;
        crouch_down = (leg_down + calf_down) * (-1);
        cout << "leg = " << leg_down << " calf = " << calf_down << " total = " << crouch_down << endl;
    }
}

//Draw robot
void draw_robo()
{
    glTranslatef(0.0, 5.0, 0.0);

    //draw body
    glPushMatrix(); //(0, 5, 0)
    glTranslatef(0.0, 5.0, 0.0);
    glRotatef(-90, 1.0, 0.0, 0.0);  //把z軸座標轉到-y方向
    if (run) {
        glRotatef(10, 1.0, 0.0, 0.0);
    }

    glColor3f(0.0, 1.0, 0.0);    /* Gray colored */
    if (cylind == NULL) {               /* allocate a quadric object, if necessary */
        cylind = gluNewQuadric();
        gluQuadricDrawStyle(cylind, GLU_FILL);
        gluQuadricNormals(cylind, GLU_SMOOTH);
    }
    gluCylinder(cylind, 4, 4,      //沿著z軸畫圓柱
        6,
        12,
        3
    );
    glPopMatrix();//回到(0, 5, 0)

    //draw right leg
    glPushMatrix();
    glTranslatef(-2.0, 5.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    if (run) {
        glTranslatef(0.0, 0.0, 1.0);
        glRotatef(-30, 1.0, 0.0, 0.0);
    }
    glRotatef(r_leg, 1.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 0.0); //Yello color
    gluCylinder(cylind, 1.5, 1.5, 2.5, 12, 3);
    //draw right knee
    glTranslatef(0.0, 0.5, 3.0);
    glPushMatrix();
    if (sphere == NULL) {
     sphere = gluNewQuadric();
     gluQuadricDrawStyle(sphere, GLU_FILL);
    }
    glColor3f(1.0, 0.0, 0.0); //Red color
    gluSphere(sphere, 1.5,   /* radius=2.0 */
        12,            /* composing of 12 slices*/
        12);           /* composing of 12 stacks */
    glPopMatrix();

    glRotatef(r_calf, 1.0, 0.0, 0.0);

    //draw calf
    glPushMatrix();
    glTranslatef(0.0, 0.0, 1.0);
    if (run) {
        glRotatef(30, 1.0, 0.0, 0.0);
    }
    glColor3f(0.0, 0.0, 1.0); //Blue color 
    gluCylinder(cylind, 1.5, 1.5, 6.0, 12, 3);
    glPopMatrix();
    glPopMatrix();

    //draw left leg
    glPushMatrix();
    glTranslatef(2.0, 5.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    if (run) {
        glTranslatef(0.0, 0.0, 1.0);
        glRotatef(-30, 1.0, 0.0, 0.0);
    }
    glRotatef(l_leg, 1.0, 0.0, 0.0);
    glColor3f(2.0, 1.0, 0.0); //Yello color
    gluCylinder(cylind, 1.5, 1.5, 2.5, 12, 3);
    //draw left knee
    glTranslatef(0.0, 0.5, 3.0);
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0); //Red color
    gluSphere(sphere, 1.5,   /* radius=2.0 */
        12,            /* composing of 12 slices*/
        12);           /* composing of 12 stacks */
    glPopMatrix();

    glRotatef(l_calf, 1.0, 0.0, 0.0);

    //draw calf
    glPushMatrix();
    glTranslatef(0.0, 0.0, 1.0);
    if (run) {
        glRotatef(30, 1.0, 0.0, 0.0);
    }
    glColor3f(0.0, 0.0, 1.0); //Blue color 
    gluCylinder(cylind, 1.5, 1.5, 6.0, 12, 3);
    glPopMatrix();
    glPopMatrix();

    //draw head
    glPushMatrix();
    glTranslatef(0.0, 13.0, 0.0);

    if (run) {
        glTranslatef(0.0, 0.0, 1.0);
        glRotatef(10, 1.0, 0.0, 0.0);
    }

    glColor3f(1.0, 1.0, 1.0);
    glutSolidCube(6);

    glTranslatef(-1.5, 0.0, 3.0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidSphere(1.0, 6, 6);
    
    glTranslatef(3.0, 0.0, 0.0);
    glutSolidSphere(1.0, 6, 6);

    glPopMatrix();
    
    //draw left shoulder
    glPushMatrix();
    glTranslatef(4.9, 10.0, 0.0);
    if (run) {
        glRotatef(10, 1.0, 0.0, 0.0);
    }
    glColor3f(0.32, 0.32, 0.32);
    glutSolidCube(3);


    glRotatef(La_ang, 1.0, 0.0, 0.0);
    glRotatef(gesture, 0.0, 0.0, 1.0);

    //draw left upper arm
    glPushMatrix();
    glTranslatef(-0.5, -1.4, -0.5);
    glRotatef(90, 1.0, 0.0, 0.0);
    draw_ret();
    //draw left elbow
    glTranslatef(0.5, 0.4, 3.0);
    glPushMatrix();
    glScalef(2.0, 2.0, 2.0);
    glColor3f(0.0, 1.0, 1.0);
    gluSphere(sphere, 0.5, 12, 12);
    glPopMatrix();

    glRotatef(Lfa_ang, 1.0, 0.0, 0.0);
    glRotatef(ges_fore, 0.0, 0.0, 1.0);

    //draw left forearm
    glPushMatrix();
    glTranslatef(-0.6, -0.4, 0.125);
    if (run) {
        glTranslatef(0.0, 0.0, 0.375);
        glRotatef(-60, 1.0, 0.0, 0.0);
    }
    draw_ret();

    if (hold && !right_hand) {
        glTranslatef(0.75, 0.0, 2.5);
        if (weapon == SWORD) draw_sword(); 
        if (weapon == LANCE) draw_lance();
    }

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    //draw right shoulder
    glPushMatrix();
    glTranslatef(-4.9, 10.0, 0.0);
    if (run) {
        glRotatef(10, 1.0, 0.0, 0.0);
    }
    glColor3f(0.32, 0.32, 0.32);
    glutSolidCube(3);

    glRotatef(Ra_ang, 1.0, 0.0, 0.0);
    glRotatef((-1) * gesture, 0.0, 0.0, 1.0);

    //draw right upper arm
    glPushMatrix();
    glTranslatef(-1.4, -1.3, -0.8);
    glRotatef(90, 1.0, 0.0, 0.0);
    draw_ret();


    //draw right elbow
    glTranslatef(0.5, 0.4, 3.0);
    glPushMatrix();
    glScalef(2.0, 2.0, 2.0);
    glColor3f(0.0, 1.0, 1.0);
    gluSphere(sphere, 0.5, 12, 12);
    glPopMatrix();


    //rotate forearm
    glRotatef(Rfa_ang, 1.0, 0.0, 0.0);
    glRotatef((-1) * ges_fore, 0.0, 1.0, 0.0);

    //draw right forearm
    glPushMatrix();
    glTranslatef(-0.5, -0.4, 0.125);
    if (run) {
        glTranslatef(0.0, 0.0, 0.375);
        glRotatef(-60, 1.0, 0.0, 0.0);
    }
    draw_ret();

    if(hold && right_hand) {
        glTranslatef(0.75, 0.0, 2.5);
        if (weapon == SWORD) draw_sword();
        if (weapon == LANCE) draw_lance();
    }
    
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

//draw sword
void draw_sword() {

    if (!hold || weapon != SWORD) {
        glRotatef(spin, 0.0, 1.0, 0.0);
        glScalef(2.0, 2.0, 2.0);
    }

    glPushMatrix();
    glRotatef(-90, 1.0, 0.0, 0.0);
    glColor3f(1.0, 0.0, 0.0);
    
    if (cylind == NULL) {               /* allocate a quadric object, if necessary */
        cylind = gluNewQuadric();
        gluQuadricDrawStyle(cylind, GLU_FILL);
        gluQuadricNormals(cylind, GLU_SMOOTH);
    }
    gluCylinder(cylind, 0.5, 0.5,      //沿著z軸畫圓柱
        2,
        12,
        3
    );

    glPushMatrix();
    glTranslatef(0.0, 0.0, 2.5);
    glScalef(3.0, 1.0, 1.0);
    glutWireCube(1);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, 0.0, 5.5);
    glScalef(2.0, 0.1, 5.0);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 0.0, 8.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(/*sqrt(3) - 1.0*/0.0, 0.0, 1.0);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

//draw lance
void draw_lance() {

    if (!hold || weapon != LANCE) {
        glRotatef(spin, 0.0, 1.0, 0.0);
        glScalef(1.5, 1.5, 1.5);
    }

    glPushMatrix();
    glRotatef(-90, 1.0, 0.0, 0.0);
    glColor3f(0.5, 0.0, 1.0);
    gluCylinder(cylind, 0.5, 0.5, 10.0, 12, 3);

    glTranslatef(0.0, 0.0, 10.0);

    glPushMatrix();
    glColor3f(1.0, 0.5, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glVertex3f(0.0, 0.0, 3.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(1.0, -1.0, 0.0);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 3.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 3.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 3.0);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}


/*-------------------------------------------------------
 * Display and Idle callback func. This func. draws three
 * cubes at proper places to simulate a solar system.
 */
void display()
{
    /*Clear previous frame and the depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*----Define the current eye position and the eye-coordinate system---*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();           /* Initialize modelview matrix */

    gluLookAt(45.0, 70.0, 55.0, 25.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    /*-------Draw the base of the windmill which is a cube----*/
    //glTranslatef(0.0, 0.0, 0.0);
    glTranslatef(25.0, 0.0, 0.0);
    glScalef(view_x, view_y, view_z);
    //glTranslatef(25.0, 0.0, 0.0);
    //glRotatef(20, 0.0, 1.0, 0.0);
    //glRotatef(-50, 1.0, 0.0, 0.0);
    glRotatef(ang_x, 1.0, 0.0, 0.0);
    glRotatef(ang_y, 0.0, 1.0, 0.0);
    glRotatef(ang_z, 0.0, 0.0, 1.0);

    glPushMatrix();
    glTranslatef(-30.0, 0.0, 0.0);
    draw_sword();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-30.0, 3.0, 10.0);
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-10.0, 0.0, 0.0);
    glVertex3f(10.0, 0.0, 0.0);
    glEnd();
    glTranslatef(10.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, -20.0);
    glVertex3f(0.0, 0.0, 20.0);
    glEnd();
    glTranslatef(0.0, 0.0, -20.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(-20.0, 0.0, 0.0);
    glEnd();
    glTranslatef(0.0, 0.0, 40.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(-20.0, 0.0, 0.0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-30.0, 0.0, 20.0);
    draw_lance();
    glPopMatrix();

    glPushMatrix();
    /*glRotatef(30.0, 0.0, 1.0, 0.0);
    glRotatef(-30.0, 1.0, 0.0, 0.0);*/
    draw_floor();

    //draw obstacle
    glTranslatef(-5.0, 0.0, 36.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glScalef(10.0, 6.0, 6.0);
    glColor3f(0.0, 1.0, 0.0);
    draw_ret();
    glPopMatrix();

    glTranslatef(test_x, test_y, test_z);
    glPushMatrix();
    
    glRotatef(turn_ang, 0.0, 1.0, 0.0);
    //glScalef(0.2, 0.2, 0.2);
    draw_robo();
    glPopMatrix();


    /*-------Swap the back buffer to the front --------*/
    glutSwapBuffers();
    glFlush(); /*--Display the results----*/
}


/*--------------------------------------------------
 * Reshape callback function which defines the size
 * of the main window when a reshape event occurrs.
 */
void my_reshape(int w, int h)
{
    width = w;
    height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-40.0, 40.0, -40, 40, 0.0, 120.0);
}

/*--------------------------------------------------
 * Keyboard callback func. When a 'q' key is pressed,
 * the program is aborted.
 */
void my_keyboard(unsigned char key, int x, int y)
{
    if (key == ' ') {
        mode = JUMP;
        jump = true;
        if (jump_s < 5) {
            jump_s++;
        }
    }
    if (key == 27) exit(0);
    if (key == '.') {
        if (!crouch) {
            test_y = -2.3;
            r_leg = l_leg = -45;
            r_calf = l_calf = 90;
            crouch = true;
        }
        else {
            test_y = 0.0;
            r_leg = l_leg = 0;
            r_calf = l_calf = 0;
            crouch = false;
        }
    }
    if (key == 'p') {
        hold = false;
        weapon = NONE;
    }
    if (key == 'g') {
        mode = GESTURE;
        ges = true;
    }
    if (key == 'o')  right_hand = !right_hand;
    if (key == 'w') mode = WALK;
    if (key == 's') mode = STOP;
    if (key == 'r') {
        run = true;
        if (run_speed < 3.0) {
            run_speed += 0.4;
        }
    }
    if (key == 'd' || key == 'D')
    {
        D = (D == true) ? false : true;
        cout << "status: " << D << endl;
    }
    if (D)
    {
        if (key == 'q') {
            if ((La_ang - 1) < -180) return;
            La_ang = (La_ang - 1) % 360;
        }
        if (key == 'e') {
            if ((Ra_ang - 1) < -180)  return;
            Ra_ang = (Ra_ang - 1) % 360;
        }
        if (key == 'z') {
            if ((Lfa_ang - 1) < -120)   return;
            Lfa_ang = (Lfa_ang - 1) % 360;
        }
        if (key == 'c') {
            if ((Rfa_ang - 1) < -120) return;
            Rfa_ang = (Rfa_ang - 1) % 360;
        }
        if (key == '1') test_x -= 0.1;
        if (key == '2') test_y -= 0.1;
        if (key == '3') test_z -= 0.1;
        if (key == '4') ang_x = (ang_x - 1) % 360;
        if (key == '5') ang_y = (ang_y - 1) % 360;
        if (key == '6') ang_z = (ang_z - 1) % 360;
        if (key == '7') r_x = (r_x - 5) % 360;
        if (key == '8') r_y = (r_y - 5) % 360;
        if (key == '9') r_z = (r_z - 5) % 360;
    }
    else
    {
        if (key == 'q') {
            if ((La_ang + 1) > 30)  return;
            La_ang = (La_ang + 1) % 360;
        }
        if (key == 'e') {
            if ((Ra_ang + 1) > 30)  return;
            Ra_ang = (Ra_ang + 1) % 360;
        }
        if (key == 'z') {
            if ((Lfa_ang + 1) > 0) return;
            Lfa_ang = (Lfa_ang + 1) % 360;
        }
        if (key == 'c') {
            if ((Rfa_ang + 1) > 0) return;
            Rfa_ang = (Rfa_ang + 1) % 360;
        }
        if (key == '1') test_x += 0.1;
        if (key == '2') test_y += 0.1;
        if (key == '3') test_z += 0.1;
        if (key == '4') ang_x = (ang_x + 1) % 360;
        if (key == '5') ang_y = (ang_y + 1) % 360;
        if (key == '6') ang_z = (ang_z + 1) % 360;
        if (key == '7') r_x = (r_x + 5) % 360;
        if (key == '8') r_y = (r_y + 5) % 360;
        if (key == '9') r_z = (r_z + 5) % 360;
    }
    //cout << "pos: (" << test_x << "," << test_y << "," << test_z << ")  \n";
    //cout << "angle: (" << ang_x << "," << ang_y << "," << ang_z << ")\n";
    //cout << "(" << l_calf << "," << r_calf << ")\n";
    display();
}

//Check boundary condition
bool CheckBoundary() {
    cout << "(" << test_x << "," << test_y << "," << test_z << ")\n";
    float distance = 0.0;
    if (!crouch && !run) {
        distance = 0.6;
    }
    if (crouch && !run) {
        distance = 0.3;
    }
    if (!crouch && run) {
        distance = 0.6 * run_speed;
    }

    if (turn_ang == 0) {
        if ((test_z + distance > 23.4 && test_z < 39.6) && (test_x >= -8.6 && test_x <= 13.6))  return false;
        if ((test_z + distance > -10.0 && test_z < 10.0) && (test_x >= -40.0 && test_x <= -20.0)) {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_z + distance > 10.0 && test_z < 30.0) && (test_x >= -40.0 && test_x <= -20.0)) {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }
    if (turn_ang == 90 || turn_ang == -270) {
        if ((test_x + distance > -8.6 && test_x < 13.6) && (test_z >= 23.4 && test_z <= 39.6))   return false;
        if ((test_x + distance > -40.0 && test_x < -20.0) && (test_z >= -10 && test_z <= 10)) {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_x + distance > -40.0 && test_x < -20.0) && (test_z >= 10 && test_z <= 30)) {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }
    if (turn_ang == 180 || turn_ang == -180) {
        if ((test_z - distance < 39.6 && test_z > 23.4) && (test_x >= -8.6 && test_x <= 13.6)) return false;
        if ((test_z - distance < 10.0 && test_z > -10.0) && (test_x >= -40.0 && test_x <= -20.0)) {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_z - distance < 30.0 && test_z > 10.0) && (test_x >= -40.0 && test_x <= -20.0)) {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }
    if (turn_ang == -90 || turn_ang == 270) {
        if ((test_x - distance < 13.6 && test_x > -8.6) && (test_z >= 23.4 && test_z <= 39.6)) return false;
        if ((test_x - distance < -20.0 && test_x > -40.0) && (test_z >= -10 && test_z <= 10)) {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_x - distance < -20.0 && test_x > -40.0) && (test_z >= 10 && test_z <= 30)) {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }

    return true;
}

//stop func
void Stop() {
    if (counter % angle_speed == 0 && !crouch) {
        if (La_ang < 0) {
            La_ang += 1;
        }
        if (r_leg < 0 && La_ang % 2 == 0) {
            r_leg += 1;
        }
        if (Ra_ang < 0) {
            Ra_ang += 1;
        }
        if (l_leg < 0 && Ra_ang % 2 == 0) {
            l_leg += 1;
        }
    }
    else if (counter % angle_speed == 0 && crouch) {
        if (r_calf < 90) {
            r_calf++;
        }
        if (l_calf < 90) {
            l_calf++;
        }
    }
    counter--;
    if (counter == 0)    stop = true;
}

//moveforward func
void Move() {
    float distance = 0.0;
    if (!crouch && !run) {
        distance = 0.6;
    }
    if (crouch && !run) {
        distance = 0.3;
    }
    if (!crouch && run) {
        distance = 0.6 * run_speed;
    }
    if (turn_ang == 0 && CheckBoundary())   test_z += distance;
    if ((turn_ang == 90 || turn_ang == -270) && CheckBoundary())  test_x += distance;
    if ((turn_ang == 180 || turn_ang == -180) && CheckBoundary())  test_z -= distance;
    if ((turn_ang == -90 || turn_ang == 270) && CheckBoundary())  test_x -= distance;
}

void movement()
{
    spin = (spin + 5) % 360;
    if (mode == STOP) {
        if (!stop) {
            run = false;
            Stop();
        }
    }
    else if (mode == WALK) {
        counter++;
        stop = false;
        if ((counter % angle_speed == 0 && !crouch) && !run) {
            if (one_side_move) {
                if (Ra_ang > -30) {
                    Ra_ang -= 1;
                }
                if (l_leg > -15 && Ra_ang % 2 == 0) {
                    l_leg -= 1;
                }
                if (La_ang < 0) {
                    La_ang += 1;
                }
                if (r_leg < 0 && La_ang % 2 == 0) {
                    r_leg += 1;
                }
                if (Ra_ang == -30 && l_leg == -15) {
                    Move();
                    one_side_move = false;
                }
            }
            else {
                if (La_ang > -30) {
                    La_ang -= 1;
                }
                if (r_leg > -15 && La_ang % 2 == 0) {
                    r_leg -= 1;
                }
                if (Ra_ang < 0) {
                    Ra_ang += 1;
                }
                if (l_leg < 0 && Ra_ang % 2 == 0) {
                    l_leg += 1;
                }
                if (La_ang == -30 && r_leg == -15) {
                    Move();
                    one_side_move = true;
                }
            }
        }
        else if ((counter % angle_speed == 0 && crouch) && !run) {
            if (one_side_move) {
                if (l_calf > 45) {
                    l_calf -= 1;
                }
                if (r_calf < 90) {
                    r_calf += 1;
                }
                if (La_ang < 0) {
                    La_ang += 1;
                }
                if (Ra_ang > -30) {
                    Ra_ang -= 1;
                }
                if (l_calf == 45) {
                    Move();
                    one_side_move = false;
                }
            }
            else {
                if (r_calf > 45) {
                    r_calf -= 1;
                }
                if (l_calf < 90) {
                    l_calf += 1;
                }
                if (Ra_ang < 0) {
                    Ra_ang += 1;
                }
                if (La_ang > -30) {
                    La_ang -= 1;
                }
                if (r_calf == 45) {
                    Move();
                    one_side_move = true;
                }
            }
        }
        else if (counter % angle_speed == 0 && run) {
            if (one_side_move) {
                if (Ra_ang > -30) {
                    Ra_ang -= 1;
                }
                if (La_ang < 15) {
                    La_ang += 1;
                }
                if (l_leg > -15) {
                    l_leg -= 1;
                }
                if (r_leg < 15) {
                    r_leg += 1;
                }
                if (Ra_ang == -30 && l_leg == -15) {
                    Move();
                    one_side_move = false;
                }
            }
            else {
                if (La_ang > -30) {
                    La_ang -= 1;
                }
                if (Ra_ang < 15) {
                    Ra_ang += 1;
                }
                if (r_leg > -15) {
                    r_leg -= 1;
                }
                if (l_leg < 15) {
                    l_leg += 1;
                }
                if (La_ang == -30 && r_leg == -15) {
                    Move();
                    one_side_move = true;
                }
            }
        }
    }
    else if (mode == JUMP) {
        if (test_y < 1.0 * jump_s && jump) {
            test_y += 0.1;
        }
        else {
            jump = false;
            jump_s = 0;
        }
        if (!jump && test_y > 0.0) {
            test_y -= 0.1;
        }
    }
    else if (mode == GESTURE) {
        Stop();
        if (gesture < 120) {
            gesture ++;
        }
        else if (ges_fore < 90) {
            ges_fore ++;
        }
        else {
            ges = false;
        }
        if (!ges) {
            if (ges_fore > 0) {
                ges_fore --;
            }
            else if (gesture > 0) {
                gesture --;
            }
        }
    }
    glutPostRedisplay();
}

/*---------------------------------------------
 * Idle event callback function, increase blade and arm
 * rotation angles.
 */
void idle_func()
{
    bld_ang += 5.0;
    if (bld_ang > 360.0) {
        bld_ang -= 360.0;
        arm_ang += 3.0;
        if (arm_ang > 360.0) arm_ang -= 360.0;
    }
    display(); /* show the scene again */
}

void mouse(int button, int state, int x, int y)
{
    // Wheel reports as button 3(scroll up) and button 4(scroll down)
    if ((button == 3) || (button == 4)) // It's a wheel event
    {
        // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
        if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
        printf("Scroll %s At %d %d\n", (button == 3) ? "Up" : "Down", x, y);
        if (button == 3)
        {
            view_x += 0.1;
            view_y += 0.1;
            view_z += 0.1;
        }
        else
        {
            if (view_x == 1.0)   return;
            view_x -= 0.1;
            view_y -= 0.1;
            view_z -= 0.1;
        }
    }
    else {  // normal button event
        printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
    }
}

void motion(int x, int y)
{

}

void specialKey(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_LEFT:
            turn_ang = (turn_ang + 90) % 360;
            break;
        case GLUT_KEY_RIGHT:
            turn_ang = (turn_ang - 90) % 360;
            break;
    }
}

/*---------------------------------------------------
 * Main procedure which defines the graphics environment,
 * the operation style, and the callback func's.
 */
void main(int argc, char** argv)
{
    /*-----Initialize the GLUT environment-------*/
    glutInit(&argc, argv);

    /*-----Depth buffer is used, be careful !!!----*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("robot");

    myinit();      /*---Initialize other state varibales----*/

    /*----Associate callback func's whith events------*/
    glutDisplayFunc(display);
    //glutIdleFunc(idle_func);    /* Add idle event */
    glutReshapeFunc(my_reshape);
    glutKeyboardFunc(my_keyboard);
    glutMouseFunc(mouse);
    glutIdleFunc(movement);
    glutSpecialFunc(specialKey);

    glutMainLoop();
}


/*
    視野直視角色
    glRotatef(20.0, 0.0, 1.0, 0.0);
    glRotatef(-50, 1.0, 0.0, 0.0);

    Debug Obj
    glTranslatef(test_x, 0.0, 0.0);
    glTranslatef(0.0, test_y, 0.0);
    glTranslatef(0.0, 0.0, test_z);

    glRotatef(r_x, 1.0, 0.0, 0.0);
    glRotatef(r_y, 0.0, 1.0, 0.0);
    glRotatef(r_z, 0.0, 0.0, 1.0);
*/