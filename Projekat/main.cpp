#include <math.h>		
#include <stdio.h>
#include <stdlib.h>	
#include <iostream>
#include <ctime>
#include <string>
#include "Glut.h"

const double Xmin = 0.0, Xmax = 768.0;
const double Ymin = 0.0, Ymax = 672.0;

const double courtdownY = Ymax / 7;
const double courtupY = Ymax - Ymax / 4;

static int scoreP1 = 0;
static int scoreP2 = 0;

static float ballX = Xmax / 2;
static float ballY = Ymax / 2;
const float ballRadius = 6;

const float hoopLeftRimY = courtupY - 48;
const float hoopLeftRimX = Xmin + 68;
const float hoopRimRadius = 12;

static time_t startTime;
static int countdownMinutes = 10;

enum Color {
    RED,
    GREEN,
    BLUE,
    BLACK,
    WHITE,
    ORANGE,
    COURT_COLOR,
    HOOP_BACKBOARD
};

const GLfloat colors[][3] = {
    {1.0f, 0.0f, 0.0f},                     // RED
    {0.0f, 1.0f, 0.0f},                     // GREEN
    {0.0f, 0.0f, 1.0f},                     // BLUE
    {0.0f, 0.0f, 0.0f},                     // BLACK
    {1.0f, 1.0f, 1.0f},                     // WHITE
    {1.0f, 0.647f, 0.0f},                   // ORANGE
    {0.4609375f, 0.4609375f, 0.46484375f},  // COURT_COLOR
    {0.53f, 0.39f, 0.18f}                   // HOOP_BACKBOARD
};

void drawStaticElements() {
    // Draw the court rectangle
    glColor3fv(colors[BLACK]);
    glLineWidth(5.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(Xmin, courtdownY);
    glVertex2f(Xmax, courtdownY);
    glVertex2f(Xmax - 100, courtupY);
    glVertex2f(Xmin + 100, courtupY);
    glEnd();

    // Draw the court center line
    glBegin(GL_LINES);
    glVertex2f(Xmax / 2, courtupY);
    glVertex2f(Xmax / 2, courtdownY);
    glEnd();

    // Draw the threepoint square left side
    glBegin(GL_LINE_STRIP);
    glVertex2f(Xmin + 58, (courtdownY + courtupY) / 2 + 30);
    glVertex2f(Xmin + 178, (courtdownY + courtupY) / 2 + 30);
    glVertex2f(Xmin + 170, (courtdownY + courtupY) / 2 - 30);
    glVertex2f(Xmin + 43, (courtdownY + courtupY) / 2 - 30);
    glEnd();

    // Draw the threepoint square right side
    glBegin(GL_LINE_STRIP);
    glVertex2f(Xmax - 58, (courtdownY + courtupY) / 2 + 30);
    glVertex2f(Xmax - 178, (courtdownY + courtupY) / 2 + 30);
    glVertex2f(Xmax - 170, (courtdownY + courtupY) / 2 - 30);
    glVertex2f(Xmax - 43, (courtdownY + courtupY) / 2 - 30);
    glEnd();

    // Draw the center line circle
    glBegin(GL_LINE_LOOP); // NE KONTAM
    for (int i = 0; i < 1080; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(360);//get the current angle

        float x = 35 * cosf(theta);//calculate the x component
        float y = 35 * sinf(theta);//calculate the y component
        glVertex2f(x + Xmax / 2, y + (courtdownY + courtupY) / 2);
    }
    glEnd();

    //HOOP LEFT
    //Hoop pole 1
    glBegin(GL_QUADS);
    glVertex2f(Xmin + 25, (courtdownY + courtupY) / 2);
    glVertex2f(Xmin + 25, courtupY - 40);
    glVertex2f(Xmin + 35, courtupY - 40);
    glVertex2f(Xmin + 35, (courtdownY + courtupY) / 2);
    glEnd();
    //Hoop pole 2
    glBegin(GL_QUADS);
    glVertex2f(Xmin + 25, courtupY - 50);
    glVertex2f(Xmin + 25, courtupY - 40);
    glVertex2f(Xmin + 50, courtupY - 40);
    glVertex2f(Xmin + 50, courtupY - 50);
    glEnd();

    //Hoop backboard NEEDS FIXING
    glColor3fv(colors[HOOP_BACKBOARD]);
    glBegin(GL_QUADS);
    glVertex2f(Xmin + 50, courtupY - 70);
    glVertex2f(Xmin + 50, courtupY - 20);
    glVertex2f(Xmin + 70, courtupY + 40);
    glVertex2f(Xmin + 70, courtupY - 10);
    glEnd();

    //Hoop rim NEEDS FIXING
    glLineWidth(1.0f);
    glColor3fv(colors[WHITE]); //???????????????????????????????????????????????????????????????????????????????????????????????
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 1080; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(360);//get the current angle

        float x = hoopRimRadius * cosf(theta);//calculate the x component
        float y = hoopRimRadius * sinf(theta);//calculate the y component
        glVertex2f(x + hoopLeftRimX, y + hoopLeftRimY);
    }
    glEnd();

    //HOOP RIGHT

    //Hoop pole 1
    glColor3fv(colors[BLACK]);
    glBegin(GL_QUADS);
    glVertex2f(Xmax - 25, (courtdownY + courtupY) / 2);
    glVertex2f(Xmax - 25, courtupY - 40);
    glVertex2f(Xmax - 35, courtupY - 40);
    glVertex2f(Xmax - 35, (courtdownY + courtupY) / 2);
    glEnd();
    //Hoop pole 2
    glBegin(GL_QUADS);
    glVertex2f(Xmax - 25, courtupY - 50);
    glVertex2f(Xmax - 25, courtupY - 40);
    glVertex2f(Xmax - 50, courtupY - 40);
    glVertex2f(Xmax - 50, courtupY - 50);
    glEnd();

    //Hoop backboard NEEDS FIXING
    glColor3fv(colors[HOOP_BACKBOARD]);
    glBegin(GL_QUADS);
    glVertex2f(Xmax - 50, courtupY - 70);
    glVertex2f(Xmax - 50, courtupY - 20);
    glVertex2f(Xmax - 70, courtupY + 40);
    glVertex2f(Xmax - 70, courtupY - 10);
    glEnd();

    //Hoop rim NEEDS FIXING
    glLineWidth(1.0f);
    glColor3fv(colors[WHITE]);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 1080; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(360);//get the current angle

        float x = 12 * cosf(theta);//calculate the x component
        float y = 12 * sinf(theta);//calculate the y component
        glVertex2f(x + Xmax - 68, y + courtupY - 48);
    }
  
    glEnd();
}

void drawBall(float x_offset, float y_offset) {
    glColor3fv(colors[ORANGE]);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 1080; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(360);//get the current angle
        float x = ballRadius * cosf(theta);//calculate the x component
        float y = ballRadius * sinf(theta);//calculate the y component
        glVertex2f(x + x_offset, y + y_offset);
    }
    glEnd();
}

void drawScore() {
    //PLAYER 1 SCORE
    glRasterPos2f(Xmax / 2 - 150, Ymax - 75);
    std::string score = "SCORE";
    for (char c : score) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glRasterPos2f(Xmax / 2 - 117, Ymax - 100);

    std::string str_scoreP1 = std::to_string(scoreP1);

    for (char c : str_scoreP1) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    //PLAYER 2 SCORE
    glRasterPos2f(Xmax / 2 + 80, Ymax - 75);

    for (char c : score) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glRasterPos2f(Xmax / 2 + 113, Ymax - 100);

    std::string str_scoreP2 = std::to_string(scoreP2);

    for (char c : str_scoreP2) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

void calculateAhdDrawTime() {
    time_t currentTime = time(NULL);
    double elapsedSeconds = difftime(currentTime, startTime);
    int remainingMinutes = countdownMinutes - 1 - static_cast<int>(elapsedSeconds) / 60;
    int remainingSeconds = 59 - static_cast<int>(elapsedSeconds) % 60;

    glRasterPos2f(Xmax / 2 - 20, Ymax - 50);

    std::string timerText = std::to_string(remainingMinutes) + ":" + (remainingSeconds < 10 ? "0" : "") + std::to_string(remainingSeconds);
    for (char c : timerText) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

void myKeyboardFunc(unsigned char key, int x, int y)
{


}

void mySpecialKeyFunc(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        ballY += 5;
        break;
    case GLUT_KEY_DOWN:
        ballY -= 5;
        break;
    case GLUT_KEY_LEFT:
        ballX -= 5;
        break;
    case GLUT_KEY_RIGHT:
        ballX += 5;
        break;
    }
}

void drawScene() {
    glClearColor(0.4609375f, 0.4609375f, 0.46484375f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    calculateAhdDrawTime();
    drawScore();
    drawBall(ballX, ballY);
    drawStaticElements();
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
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
void timer(int value) {

    // Update ball position
    ballY -=2.0f;

    // Check if ball passes through the hoop
    if (ballY - ballRadius < hoopLeftRimY + hoopRimRadius && ballY + ballRadius > hoopLeftRimY - hoopRimRadius) {
        float distance = abs(static_cast<long> (ballX - hoopLeftRimX));
        if (distance < hoopRimRadius) {
            // Ball passed through the hoop
            scoreP1 += 1;
            printf("Ball passed through the hoop! %d\n", scoreP1);
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(10, 60);
    glutInitWindowSize(768, 672);

    //preimenovati u Kolokvijum_ime_prezime (npr. Kolokvijum_Tijana_Sustersic)
    glutCreateWindow("Kolokvijum_ime_prezime");

    initRendering();

    glutKeyboardFunc(myKeyboardFunc);
    glutSpecialFunc(mySpecialKeyFunc);


    glutReshapeFunc(resizeWindow);

    glutDisplayFunc(drawScene);

    glutTimerFunc(0, timer, 0);

    startTime = time(NULL);

    glutMainLoop();

    return(0);
}
