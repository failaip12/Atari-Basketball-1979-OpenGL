#define STB_IMAGE_IMPLEMENTATION
#include <math.h>		
#include <stdio.h>
#include <stdlib.h>	
#include <iostream>
#include <ctime>
#include <string>
#include "Glut.h"
#include "stb_image.h"

const double Xmin = 0.0, Xmax = 768.0;
const double Ymin = 0.0, Ymax = 672.0;

const double courtdownY = Ymax / 7;
const double courtupY = Ymax - Ymax / 4;

static int scoreP1 = 0;
static int scoreP2 = 0;

static float ballX = Xmax / 2;
static float ballY = Ymax / 2;
const float ballRadius = 6;

const float hoopRimY = courtupY - 48;
const float hoopLeftRimX = Xmin + 68;
const float hoopRightRimX = Xmax - 68;
const float hoopRimRadius = 12;

static time_t startTime;
static int countdownMinutes = 10;

GLuint playerTexture;
int a;

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

GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);


    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);

    if (image) {
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(image);
    }
    else {
        printf("Failed to load texture: %s\n", filename);
        stbi_image_free(image);
        return 0;
    }

    return textureID;
}
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
        float theta = 2.0f * 3.1415926f * float(i) / float(360);

        float x = 35 * cosf(theta);
        float y = 35 * sinf(theta);
        glVertex2f(x + Xmax / 2, y + (courtdownY + courtupY) / 2);
    }
    glEnd();

    //HOOP LEFT
    //Hoop pole 1


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    glBegin(GL_QUADS);
    glVertex2f(Xmin + 25, (courtdownY + courtupY) / 2);
    glVertex2f(Xmin + 25, courtupY - 40);
    glVertex2f(Xmin + 35, courtupY - 40);
    glVertex2f(Xmin + 35, (courtdownY + courtupY) / 2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
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
        glVertex2f(x + hoopLeftRimX, y + hoopRimY);
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
        glVertex2f(x + hoopRightRimX, y + hoopRimY);
    }

    glEnd();
}

void drawBall(float x_offset, float y_offset) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    glPushMatrix();
    glColor3fv(colors[ORANGE]);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(360); // Get the current angle
        float x = ballRadius * cosf(theta); // Calculate the x component
        float y = ballRadius * sinf(theta); // Calculate the y component

        float s = (x + ballRadius) / (2 * ballRadius); // Calculate the texture S coordinate
        float t = (y + ballRadius) / (2 * ballRadius); // Calculate the texture T coordinate

        glTexCoord2f(s, t);
        glVertex2f(x + x_offset, y + y_offset);
    }
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawScore() {
    glColor3fv(colors[WHITE]);
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

void calculateAndDrawTime() {
    glColor3fv(colors[WHITE]);
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
    glClear(GL_COLOR_BUFFER_BIT);
    calculateAndDrawTime();
    drawScore();
    drawStaticElements();
    drawBall(ballX, ballY);
    glutSwapBuffers();
    glutPostRedisplay();
}

void initRendering()
{
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f);
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
    gluOrtho2D(windowXmin, windowXmax, windowYmin, windowYmax);

}
// Declare a flag to track if the ball has passed through the hoop
bool ballPassedThrough = false;

void timer(int value) {

    // Update ball position
    ballY -= 0.50f;

    // Check if the ball is within the range of the hoop
    bool ballInRange = (ballY - ballRadius < hoopRimY + hoopRimRadius) && (ballY + ballRadius > hoopRimY - hoopRimRadius);

    if (ballInRange) {
        // Check if the ball passes through the left rim of the hoop
        float distanceToLeftRim = abs(ballX - hoopLeftRimX);
        if (distanceToLeftRim < hoopRimRadius && !ballPassedThrough) {
            // Ball passed through the hoop
            scoreP1 += 1;

            // Set the flag to prevent multiple score increments
            ballPassedThrough = true;
        }

        // Check if the ball passes through the right rim of the hoop
        float distanceToRightRim = abs(ballX - hoopRightRimX);
        if (distanceToRightRim < hoopRimRadius && !ballPassedThrough) {
            // Ball passed through the hoop
            scoreP2 += 1;

            // Set the flag to prevent multiple score increments
            ballPassedThrough = true;
        }
    }

    // Reset the flag if the ball is not within the range of the hoop
    if (!ballInRange) {
        ballPassedThrough = false;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(10, 60);
    glutInitWindowSize(Xmax, Ymax);

    glutCreateWindow("Kolokvijum_ime_prezime");
    playerTexture = loadTexture("C:/Users/datcha/source/repos/failaip12/Projekat-Grafika/Projekat/pics/shark2.png");
    if (playerTexture == 0) {
        return 1;
    }
    //preimenovati u Kolokvijum_ime_prezime (npr. Kolokvijum_Tijana_Sustersic)

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