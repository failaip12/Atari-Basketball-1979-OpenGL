#define STB_IMAGE_IMPLEMENTATION
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include "Glut.h"
#include "stb_image.h"

const double WINDOW_WIDTH = 768.0;
const double WINDOW_HEIGHT = 672.0;

const double courtdownY = WINDOW_HEIGHT / 7;
const double courtupY = WINDOW_HEIGHT - WINDOW_HEIGHT / 4;

static int scoreP1 = 0;
static int scoreP2 = 0;

static float ballX = WINDOW_WIDTH / 2;
static float ballY = WINDOW_HEIGHT / 2;
const float ballRadius = 6;
static float ballSpeedX = 0.0;
static float ballSpeedY = 0.0;

const float hoopRimY = courtupY - 48;
const float hoopLeftRimX = 68;
const float hoopRightRimX = WINDOW_WIDTH - 68;
const float hoopRimRadius = 12;
static bool ballPassedThrough = false;

static time_t startTime;
static int countdownMinutes = 10;

GLuint playerTexture;

const int pole_height = 154;

static float player1X = WINDOW_WIDTH / 2 + 100;
static float player1Y = WINDOW_HEIGHT / 2 + 100;
const int playerWidth = 120;
const int playerHeight = 120;

static bool ballHeld = false;
static float jumpAmplitude = 19.0;
static float jumpFrequency = 5.0;
static float time_ball = 0.0;
const float TIME_INCREMENT = 0.01;
static float jump_offset;
static float gravity = 0.1;

static bool shotFired = false;
static bool shootKeyHeld = false;
static float shootStrength = 0.0;


static bool keyState[256] = { false };
static bool specialKeyState[256] = { false };

struct Movement {
    char key[2];
    float deltaY;
    float deltaX;
};
Movement player1Movements[] = {
    {{'w', 'W'}, 4, 0},
    {{'s', 'S'}, -4, 0},
    {{'a', 'A'}, 0, -4},
    {{'d', 'D'}, 0, 4}
};
Movement player2Movements[] = {
    {{'i', 'I'}, 4, 0},
    {{'k', 'K'}, -4, 0},
    {{'j', 'J'}, 0, -4},
    {{'l', 'L'}, 0, 4}
};

enum Color {
    RED,
    GREEN,
    BLUE,
    BLACK,
    WHITE,
    ORANGE,
    HOOP_BACKBOARD
};

const GLfloat colors[][3] = {
    {1.0, 0.0, 0.0},                     // RED
    {0.0, 1.0, 0.0},                     // GREEN
    {0.0, 0.0, 1.0},                     // BLUE
    {0.0, 0.0, 0.0},                     // BLACK
    {1.0, 1.0, 1.0},                     // WHITE
    {1.0, 0.647, 0.0},                   // ORANGE
    {0.53, 0.39, 0.18}                   // HOOP_BACKBOARD
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

void shootBall(float shotStrength) {
    if (!shotFired) {
        float minSpeed = 1.0;
        float maxSpeed = 10.0;
        float minAngle = 45.0;
        float maxAngle = 75.0;

        float shotSpeed = minSpeed + (shotStrength * (maxSpeed - minSpeed));
        shotSpeed = fminf(fmaxf(shotSpeed, minSpeed), maxSpeed);

        float shotAngle = minAngle + (shotStrength * (maxAngle - minAngle));
        shotAngle = fminf(fmaxf(shotAngle, minAngle), maxAngle);

        float radians = shotAngle * M_PI / 180.0;

        ballSpeedX = fabsf(shotSpeed * cosf(radians));
        ballSpeedY = fabsf(shotSpeed * sinf(radians));
        shootStrength = 0.0;
        //printf("X:%f, Y:%f\n", shotSpeed, ballSpeedY);
    }
}

bool checkPlayerBallCollision(float playerX, float playerY, float ballX, float ballY, float playerWidth, float playerHeight, float ballRadius)
{
    // Calculate the boundaries of the player rectangle
    float playerLeft = playerX - playerWidth / 2;
    float playerRight = playerX + playerWidth / 2;
    float playerTop = playerY - playerHeight / 2;
    float playerBottom = playerY + playerHeight / 2;

    // Calculate the boundaries of the ball rectangle
    float ballLeft = ballX - ballRadius;
    float ballRight = ballX + ballRadius;
    float ballTop = ballY - ballRadius;
    float ballBottom = ballY + ballRadius;

    // Check for overlap between the rectangles
    if (playerLeft <= ballRight && playerRight >= ballLeft && playerTop <= ballBottom && playerBottom >= ballTop) {
        return true; // Player touched the ball
    }

    return false; // Player did not touch the ball
}

void drawCircle(int GLprimitive, float centerX, float centerY, float radius) {
    glBegin(GLprimitive);
    for (int i = 0; i < 1080; ++i) {
        float theta = i * M_PI / 180.0;
        float x = radius * cos(theta);
        float y = radius * sin(theta);
        glVertex2f(x + centerX, y + centerY);
    }
    glEnd();
}

void drawStaticElements() {
    // Draw the court rectangle
    glColor3fv(colors[BLACK]);
    glLineWidth(5.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.0, courtdownY);
    glVertex2f(WINDOW_WIDTH, courtdownY);
    glVertex2f(WINDOW_WIDTH - 100, courtupY);
    glVertex2f(100, courtupY);
    glEnd();

    // Draw the court center line
    glBegin(GL_LINES);
    glVertex2f(WINDOW_WIDTH / 2, courtupY);
    glVertex2f(WINDOW_WIDTH / 2, courtdownY);
    glEnd();

    // Draw the threepoint square left side
    glBegin(GL_LINE_STRIP);
    glVertex2f(58, (courtdownY + courtupY) / 2 + 30);
    glVertex2f(178, (courtdownY + courtupY) / 2 + 30);
    glVertex2f(170, (courtdownY + courtupY) / 2 - 30);
    glVertex2f(43, (courtdownY + courtupY) / 2 - 30);
    glEnd();

    // Draw the threepoint square right side
    glBegin(GL_LINE_STRIP);
    glVertex2f(WINDOW_WIDTH - 58, (courtdownY + courtupY) / 2 + 30);
    glVertex2f(WINDOW_WIDTH - 178, (courtdownY + courtupY) / 2 + 30);
    glVertex2f(WINDOW_WIDTH - 170, (courtdownY + courtupY) / 2 - 30);
    glVertex2f(WINDOW_WIDTH - 43, (courtdownY + courtupY) / 2 - 30);
    glEnd();


    // Draw the center line circle
    drawCircle(GL_LINE_LOOP, WINDOW_WIDTH / 2, (courtdownY + courtupY) / 2, 35);

    //HOOP LEFT
    //Hoop pole 1
    glBegin(GL_QUADS);
    glVertex2f(25, (courtdownY + courtupY) / 2);
    glVertex2f(25, (courtdownY + courtupY) / 2 + pole_height);
    glVertex2f(35, (courtdownY + courtupY) / 2 + pole_height);
    glVertex2f(35, (courtdownY + courtupY) / 2);
    glEnd();
    //Hoop pole 2
    glBegin(GL_QUADS);
    glVertex2f(25, courtupY - 50);
    glVertex2f(25, courtupY - 40);
    glVertex2f(50, courtupY - 40);
    glVertex2f(50, courtupY - 50);
    glEnd();

    //Hoop backboard NEEDS FIXING
    glColor3fv(colors[HOOP_BACKBOARD]);
    glBegin(GL_QUADS);
    glVertex2f(50, courtupY - 70);
    glVertex2f(50, courtupY - 20);
    glVertex2f(70, courtupY + 40);
    glVertex2f(70, courtupY - 10);
    glEnd();

    //Hoop rim NEEDS FIXING
    glLineWidth(1.0);
    glColor3fv(colors[WHITE]);
    drawCircle(GL_LINE_LOOP, hoopLeftRimX, hoopRimY, hoopRimRadius);

    //HOOP RIGHT

    //Hoop pole 1
    glColor3fv(colors[BLACK]);
    glBegin(GL_QUADS);
    glVertex2f(WINDOW_WIDTH - 25, (courtdownY + courtupY) / 2);
    glVertex2f(WINDOW_WIDTH - 25, (courtdownY + courtupY) / 2 + pole_height);
    glVertex2f(WINDOW_WIDTH - 35, (courtdownY + courtupY) / 2 + pole_height);
    glVertex2f(WINDOW_WIDTH - 35, (courtdownY + courtupY) / 2);
    glEnd();
    //Hoop pole 2
    glBegin(GL_QUADS);
    glVertex2f(WINDOW_WIDTH - 25, courtupY - 50);
    glVertex2f(WINDOW_WIDTH - 25, courtupY - 40);
    glVertex2f(WINDOW_WIDTH - 50, courtupY - 40);
    glVertex2f(WINDOW_WIDTH - 50, courtupY - 50);
    glEnd();

    //Hoop backboard NEEDS FIXING
    glColor3fv(colors[HOOP_BACKBOARD]);
    glBegin(GL_QUADS);
    glVertex2f(WINDOW_WIDTH - 50, courtupY - 70);
    glVertex2f(WINDOW_WIDTH - 50, courtupY - 20);
    glVertex2f(WINDOW_WIDTH - 70, courtupY + 40);
    glVertex2f(WINDOW_WIDTH - 70, courtupY - 10);
    glEnd();

    //Hoop rim NEEDS FIXING
    glLineWidth(1.0f);
    glColor3fv(colors[WHITE]);
    drawCircle(GL_LINE_LOOP, hoopRightRimX, hoopRimY, 12);
}

void drawBall(float x_offset, float y_offset) {
    glColor3fv(colors[ORANGE]);
    drawCircle(GL_POLYGON, x_offset, y_offset, ballRadius);
}

void drawPlayer1(float x_offset, float y_offset) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(x_offset, playerHeight + y_offset);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(x_offset, y_offset);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(playerWidth + x_offset, y_offset);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(playerWidth + x_offset, playerHeight + y_offset);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawText(const std::string& text, float x, float y) {
    for (char c : text) {
        glRasterPos2f(x, y);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
        x += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

void drawScore() {
    glColor3fv(colors[WHITE]);

    std::string score = "SCORE";
    float player1X = WINDOW_WIDTH / 2 - 150;
    float player1Y = WINDOW_HEIGHT - 75;
    drawText(score, player1X, player1Y);

    std::string str_scoreP1 = std::to_string(scoreP1);
    float scoreP1X = WINDOW_WIDTH / 2 - 117;
    float scoreP1Y = WINDOW_HEIGHT - 100;
    drawText(str_scoreP1, scoreP1X, scoreP1Y);

    float player2X = WINDOW_WIDTH / 2 + 80;
    float player2Y = WINDOW_HEIGHT - 75;
    drawText(score, player2X, player2Y);

    std::string str_scoreP2 = std::to_string(scoreP2);
    float scoreP2X = WINDOW_WIDTH / 2 + 113;
    float scoreP2Y = WINDOW_HEIGHT - 100;
    drawText(str_scoreP2, scoreP2X, scoreP2Y);
}

void calculateAndDrawTime() {
    glColor3fv(colors[WHITE]);
    time_t currentTime = time(NULL);
    double elapsedSeconds = difftime(currentTime, startTime);
    int remainingMinutes = countdownMinutes - 1 - static_cast<int>(elapsedSeconds) / 60;
    int remainingSeconds = 59 - static_cast<int>(elapsedSeconds) % 60;

    std::string timerText = std::to_string(remainingMinutes) + ":" + (remainingSeconds < 10 ? "0" : "") + std::to_string(remainingSeconds);

    float timerX = WINDOW_WIDTH / 2 - 20;
    float timerY = WINDOW_HEIGHT - 50;
    drawText(timerText, timerX, timerY);
}

void myKeyboardFunc(unsigned char key, int x, int y)
{
    keyState[key] = true;
}
void myKeyboardFuncUp(unsigned char key, int x, int y)
{
    keyState[key] = false;
}

void mySpecialKeyFunc(int key, int x, int y)
{
    specialKeyState[key] = true;
}

void mySpecialKeyFuncUp(int key, int x, int y)
{
    specialKeyState[key] = false;
}


void drawScene() {
    glClearColor(0.4609375, 0.4609375, 0.46484375, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    calculateAndDrawTime();
    drawScore();
    drawStaticElements();
    drawBall(ballX, ballY + jump_offset);
    drawPlayer1(player1X, player1Y);
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
    if ((WINDOW_WIDTH / w) < (WINDOW_HEIGHT / h)) {
        scale = ((WINDOW_HEIGHT / h) / (WINDOW_WIDTH / w));
        center = WINDOW_WIDTH/ 2;
        windowXmin = center - (center* scale);
        windowXmax = center + (WINDOW_WIDTH - center) * scale;
        windowYmin = 0.0;
        windowYmax = WINDOW_HEIGHT;
    }
    else {
        scale = ((WINDOW_WIDTH / w) / (WINDOW_HEIGHT / h));
        center = WINDOW_HEIGHT / 2;
        windowYmin = center - (center * scale);
        windowYmax = center + (WINDOW_HEIGHT - center) * scale;
        windowXmin = 0.0;
        windowXmax = WINDOW_WIDTH;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(windowXmin, windowXmax, windowYmin, windowYmax);

}

void handleInput() {
    // Player 1 movement
    for (const auto& movement : player1Movements) {
        for (const auto& key : movement.key) {
            if (keyState[key]) {
                player1Y += movement.deltaY;
                player1X += movement.deltaX;
                break;
            }
        }
    }

    // Player 2 movement
    for (const auto& movement : player2Movements) {
        for (const auto& key : movement.key) {
            if (keyState[key]) {
                ballY += movement.deltaY;
                ballX += movement.deltaX;
                break;
            }
        }
    }

    // Player 1 shooting
    if (keyState['f'] || keyState['F']) {
        shootKeyHeld = true;
    }
    // Player 1 shooting release
    if (!keyState['f'] && !keyState['F'] && shootKeyHeld) {
        shootKeyHeld = false;
        if (!shotFired) {
            shootBall(shootStrength);
            shotFired = true;
            ballHeld = false;
        }
    }


    // Player 2 shooting
    if (keyState['l']) {
        // 'l' key is held for Player 2
        // Perform shooting action for Player 2
    }

    // Player 1 jumping
    if (keyState[32]) {
        // Space key is held for Player 1
        // Perform jumping action for Player 1
    }

    // Player 2 jumping
    if (keyState['k']) {
        // 'k' key is held for Player 2
        // Perform jumping action for Player 2
    }

    // Player 1 ball stealing
    if (keyState['r']) {
        // 'r' key is held for Player 1
        // Perform ball stealing action for Player 1
    }

    // Player 2 ball stealing
    if (keyState['j']) {
        // 'j' key is held for Player 2
        // Perform ball stealing action for Player 2
    }
}

void checkBallCollision() {
    // Check collision with the backboard
    if (ballY <= courtupY - 70 && ballY >= courtupY - 20) {

        if (ballX >= 50 && ballX <= 70) {
            // Ball collided with the backboard
            ballSpeedX *= -1; // Reverse the horizontal direction of the ball
        }
    }

    // Check collision with the rim
    float rimDistanceX = fabs(ballX - hoopLeftRimX);
    float rimDistanceY = fabs(ballY - hoopRimY);
    float distanceToRim = sqrt(pow(rimDistanceX, 2) + pow(rimDistanceY, 2));

    if (distanceToRim <= hoopRimRadius + ballRadius) {
        // Ball collided with the rim
        ballSpeedX *= -1; // Reverse the horizontal direction of the ball
        ballSpeedY *= -1; // Reverse the vertical direction of the ball
    }
}

void timer(int value) {
    //printf("X:%f, Y:%f\n", player1X, player1Y);
    //printf("shotFired: %d, shootKeyHeld: %d\n", shotFired, shootKeyHeld);
    //printf("ballSpeedX:%f, ballSpeedY:%f\n", ballSpeedX, ballSpeedY);
    handleInput();
    checkBallCollision();

    if (ballX - ballRadius < 0.0 || ballX + ballRadius > WINDOW_WIDTH) {
        ballSpeedX = -ballSpeedX;
    }

    if (ballY - ballRadius < 0.0 || ballY + ballRadius > WINDOW_HEIGHT) {
        ballSpeedY = -ballSpeedY;
    }

    if (ballHeld) {
        jump_offset = jumpAmplitude * sinf(2.0 * M_PI * jumpFrequency * time_ball);
    }
    else {
        jump_offset = 0.0;
    }

    if (shotFired) {
        ballX += ballSpeedX;
        ballY += ballSpeedY;
        ballSpeedY -= gravity;

        if (ballY - ballRadius <= 0.0) {
            ballSpeedX = 0.0f;
            ballSpeedY = 0.0f;
            shotFired = false;
        }
    }

    if (shootKeyHeld && !shotFired) {
        shootStrength += 0.035;
    }

    time_ball += TIME_INCREMENT;

    bool ballInRange = (ballY - ballRadius < hoopRimY + hoopRimRadius) && (ballY + ballRadius > hoopRimY - hoopRimRadius);
    bool touched = checkPlayerBallCollision(player1X, player1Y, ballX, ballY, playerWidth, playerHeight, ballRadius);

    if (touched) {
        ballHeld = true;
        shotFired = false;
        ballX = player1X + 70;
        ballY = player1Y + 40;
    }

    if (ballHeld) {
        ballX = player1X + 70;
        ballY = player1Y + 40;
    }

    if (ballInRange) {
        float distanceToLeftRim = abs(ballX - hoopLeftRimX);
        if (distanceToLeftRim < hoopRimRadius && !ballPassedThrough) {
            scoreP1 += 1;
            ballPassedThrough = true;
        }

        float distanceToRightRim = abs(ballX - hoopRightRimX);
        if (distanceToRightRim < hoopRimRadius && !ballPassedThrough) {
            scoreP2 += 1;
            ballPassedThrough = true;
        }
    }

    if (!ballInRange) {
        ballPassedThrough = false;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowPosition(10, 60);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    glutCreateWindow("Basketball");
    playerTexture = loadTexture("C:/Users/datcha/source/repos/failaip12/Projekat-Grafika/Projekat/pics/1.png");
    if (playerTexture == 0) {
        return 1;
    }

    initRendering();

    glutKeyboardFunc(myKeyboardFunc);
    glutKeyboardUpFunc(myKeyboardFuncUp);
    glutSpecialFunc(mySpecialKeyFunc);
    glutSpecialUpFunc(mySpecialKeyFuncUp);

    glutReshapeFunc(resizeWindow);

    glutDisplayFunc(drawScene);

    glutTimerFunc(0, timer, 0);
    startTime = time(NULL);

    glutMainLoop();

    return(0);
}