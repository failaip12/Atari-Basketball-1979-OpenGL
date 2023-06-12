#include <math.h>		
#include <stdio.h>
#include <stdlib.h>		
#include "Glut.h"

const double Xmin = 0.0, Xmax = 768.0;
const double Ymin = 0.0, Ymax = 672.0;

void myKeyboardFunc(unsigned char key, int x, int y)
{


}

void mySpecialKeyFunc(int key, int x, int y)
{

}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the court colors
    float courtColorR = 0.4609375f;
    float courtColorG = 0.4609375f;
    float courtColorB = 0.46484375f;

    // Set line colors
    float lineColorR = 0.0f;
    float lineColorG = 0.0f;
    float lineColorB = 0.0f;

    // Draw the court rectangle
    glColor3f(lineColorR, lineColorG, lineColorB);
    glLineWidth(5.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(Xmin, Ymax / 7);
    glVertex2f(Xmax, Ymax / 7);
    glVertex2f(Xmax - 100, Ymax - Ymax / 4);
    glVertex2f(Xmin + 100, Ymax - Ymax / 4);
    glEnd();

    // Draw the court center line
    glBegin(GL_LINES);
    glVertex2f(Xmax / 2, Ymax - Ymax / 4);
    glVertex2f(Xmax / 2, Ymax / 7);
    glEnd();

    // Draw the threepoint square left side
    glBegin(GL_LINE_STRIP);
    glVertex2f(Xmin + 68, Ymax - (Ymax / 2.25));
    glVertex2f(Xmin + 188, Ymax - (Ymax / 2.25));
    glVertex2f(Xmin + 180, Ymax / 2.2);
    glVertex2f(Xmin + 53, Ymax / 2.2);
    glEnd();

    // Draw the threepoint square right side
    glBegin(GL_LINE_STRIP);
    glVertex2f(Xmax - 68, Ymax - (Ymax / 2.25));
    glVertex2f(Xmax - 188, Ymax - (Ymax / 2.25));
    glVertex2f(Xmax - 180, Ymax / 2.2);
    glVertex2f(Xmax - 53, Ymax / 2.2);
    glEnd();

    // Draw the center line circle
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 1080; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(360);//get the current angle

        float x = 35 * cosf(theta);//calculate the x component
        float y = 35 * sinf(theta);//calculate the y component
        glVertex2f(x + Xmax / 2, y + Ymax / 2);
    }
    glEnd();

    // Draw the court background
    glColor3f(courtColorR, courtColorG, courtColorB);
    glBegin(GL_QUADS);
    glVertex2f(Xmin, Ymin);
    glVertex2f(Xmax, Ymin);
    glVertex2f(Xmax, Ymax);
    glVertex2f(Xmin, Ymax);
    glEnd();

    glFlush();
}

void initRendering()
{
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void resizeWindow(int w, int h)
{
    double scale, center;
    double windowXmin, windowXmax, windowYmin, windowYmax;

    glViewport(0, 0, w, h);

    w = (w == 0) ? 1 : w;
    h = (h == 0) ? 1 : h;
    if ((Xmax - Xmin) / w < (Ymax - Ymin) / h) {
        scale = ((Ymax - Ymin) / h) / ((Xmax - Xmin) / w);
        center = (Xmax + Xmin) / 2;
        windowXmin = center - (center - Xmin) * scale;
        windowXmax = center + (Xmax - center) * scale;
        windowYmin = Ymin;
        windowYmax = Ymax;
    }
    else {
        scale = ((Xmax - Xmin) / w) / ((Ymax - Ymin) / h);
        center = (Ymax + Ymin) / 2;
        windowYmin = center - (center - Ymin) * scale;
        windowYmax = center + (Ymax - center) * scale;
        windowXmin = Xmin;
        windowXmax = Xmax;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(windowXmin, windowXmax, windowYmin, windowYmax, -1, 1);

}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(10, 60);
    glutInitWindowSize(768, 672);

    //preimenovati u Kolokvijum_ime_prezime (npr. Kolokvijum_Tijana_Sustersic)
    glutCreateWindow("Kolokvijum_ime_prezime");

    initRendering();

    glutKeyboardFunc(myKeyboardFunc);
    glutSpecialFunc(mySpecialKeyFunc);


    glutReshapeFunc(resizeWindow);


    glutDisplayFunc(drawScene);

    glutMainLoop();

    return(0);
}
