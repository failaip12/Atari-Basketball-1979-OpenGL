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
const double courtCenterY = (courtupY + courtdownY) / 2;

static int scoreP1 = 0;
static int scoreP2 = 0;

static float ballX = WINDOW_WIDTH / 2;
static float ballY = courtCenterY;
const float ballRadius = 6;
static float ballSpeedX = 0.0;
static float ballSpeedY = 0.0;
static bool touchedP1 = false;
static bool touchedP2 = false;

const float hoopRimY = courtupY - 48;
const float hoopLeftRimX = 68;
const float hoopRightRimX = WINDOW_WIDTH - 68;
const float hoopRimRadius = 12;
static bool ballPassedThrough = false;
static bool ballInRange = false;

static time_t startTime;
static int countdownMinutes = 1;

GLuint player1Texture;
GLuint player2Texture;

const int pole_height = 154;

const int playerWidth = 120;
const int playerHeight = 120;

static float jumpAmplitude = 19.0;
static float jumpFrequency = 7.0;
static float time_ball = 0.0;
const float TIME_INCREMENT = 0.01;
static float jump_offset;
static float gravity = 0.1;

static bool shotFired = false;

static bool keyState[256] = { false };
static bool specialKeyState[256] = { false };

static bool player1StealButtonPressed = false;
static bool player2StealButtonPressed = false;

const float jumpDuration = 0.5;
const float jumpHeight = 80.0;

static bool endGame = false;

class Player {
private:
    float posX;
    float posY;
    bool flipped;
    bool jumping;
    float jumpStartTime;
    float jumpStartY;
    bool shootKeyHeld;
    bool ballPossesion;
    float shootStrength;

public:
    Player(float initialX, float initialY, bool flipped)
        : posX(initialX), posY(initialY), flipped(flipped), jumping(false),
        jumpStartTime(0.0), jumpStartY(0.0), shootKeyHeld(false), ballPossesion(false), shootStrength(0.0) {}

    float getPositionX() const { return posX; }
    float getPositionY() const { return posY; }
    bool isFlipped() const { return flipped; }
    bool isJumping() const { return jumping; }
    float getJumpStartTime() const { return jumpStartTime; }
    float getJumpStartY() const { return jumpStartY; }
    bool getShootKeyHeld() const { return shootKeyHeld; }
    bool getBallPossesion() const { return ballPossesion; }
    float getShootStrength() const { return shootStrength; }

    void setPositionX(float newX) { posX = newX; }
    void setPositionY(float newY) { posY = newY; }
    void setFlipped(bool isFlipped) { flipped = isFlipped; }
    void setJumping(bool isJumping) { jumping = isJumping; }
    void setJumpStartTime(float time) { jumpStartTime = time; }
    void setJumpStartY(float startY) { jumpStartY = startY; }
    void setShootKeyHeld(bool shootHeld) { shootKeyHeld = shootHeld; }
    void setBallPossesion(bool possesion) { ballPossesion = possesion; }
    void setShootStrength(float strength) { shootStrength = strength; }

    void move(float deltaX, float deltaY) {
        posX += deltaX;
        posY += deltaY;
    }

    void shootBall(float shotStrength) {
        if(ballPossesion) {
            if (!shotFired) {
                float minSpeed = 1.0;
                float maxSpeed = 15.0;
                float minAngle = 45.0;
                float maxAngle = 75.0;

                float shotSpeed = minSpeed + (shotStrength * (maxSpeed - minSpeed));
                shotSpeed = fminf(fmaxf(shotSpeed, minSpeed), maxSpeed);

                float shotAngle = minAngle + (shotStrength * (maxAngle - minAngle));
                shotAngle = fminf(fmaxf(shotAngle, minAngle), maxAngle);

                float radians = shotAngle * M_PI / 180.0;
                if (!flipped) {
                    ballSpeedX = fabsf(shotSpeed * cosf(radians));
                }
                else {
                    ballSpeedX = -fabsf(shotSpeed * cosf(radians));
                }
                ballSpeedY = fabsf(shotSpeed * sinf(radians));
                shotFired = true;
                ballPossesion = false;
                shootStrength = 0.0;
                //printf("X:%f, Y:%f\n", ballSpeedX, ballSpeedY);
            }
        }
    }

};

Player player1(hoopLeftRimX - 28, courtCenterY, false);
Player player2(hoopRightRimX - 80, courtCenterY, true);

struct Movement {
    char key[2];
    float deltaY;
    float deltaX;
    bool flipped;
};

Movement player1Movements[] = {
    {{'w', 'W'}, 4, 0},
    {{'s', 'S'}, -4, 0, false},
    {{'a', 'A'}, 0, -4, true},
    {{'d', 'D'}, 0, 4}
};

Movement player2Movements[] = {
    {{'i', 'I'}, 4, 0},
    {{'k', 'K'}, -4, 0, false},
    {{'j', 'J'}, 0, -4, true},
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

bool checkPlayerBallCollision(float playerX, float playerY, float ballX, float ballY, float playerWidth, float playerHeight, float ballRadius, bool flipped)
{
    // Calculate the boundaries of the player rectangle

    float playerTop = playerY - playerHeight / 2;
    float playerBottom = playerY + playerHeight / 2;
    float playerLeft;
    float playerRight;
    if (!flipped) {
        playerLeft = playerX - playerWidth / 2;
        playerRight = playerX + playerWidth / 2;
    }
    else {
        playerLeft = playerX + playerWidth / 2;
        playerRight = playerX - playerWidth / 2;
    }
    // Calculate the boundaries of the ball rectangle
    float ballTop = ballY - ballRadius;
    float ballBottom = ballY + ballRadius;
    float ballLeft = ballX - ballRadius;
    float ballRight = ballX + ballRadius;

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
    drawCircle(GL_LINE_LOOP, hoopRightRimX, hoopRimY, hoopRimRadius);
}

void drawBall(float x_offset, float y_offset) {
    glColor3fv(colors[ORANGE]);
    drawCircle(GL_POLYGON, x_offset, y_offset, ballRadius);
}

void drawPlayer(GLuint playerTexture, float x_offset, float y_offset, bool flipped) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    glPushMatrix();
    glBegin(GL_QUADS);
    glTexCoord2f(flipped, 1.0);
    glVertex2f(x_offset, playerHeight + y_offset);
    glTexCoord2f(flipped, 0.0);
    glVertex2f(x_offset, y_offset);
    glTexCoord2f(!flipped, 0.0);
    glVertex2f(playerWidth + x_offset, y_offset);
    glTexCoord2f(!flipped, 1.0);
    glVertex2f(playerWidth + x_offset, playerHeight + y_offset);
    glEnd();
    glPopMatrix();
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

void resetPlayer(Player& currentPlayer, float posX, float posY, bool ballPossession, bool flipped) {
    currentPlayer.setPositionX(posX);
    currentPlayer.setPositionY(posY);
    currentPlayer.setBallPossesion(ballPossession);
    currentPlayer.setFlipped(flipped);
    shotFired = false;
}

void restartGame() {
    scoreP1 = 0;
    scoreP2 = 0;

    touchedP1 = false;
    touchedP2 = false;

    ballPassedThrough = false;
    ballInRange = false;

    ballX = WINDOW_WIDTH / 2;
    ballY = courtCenterY;
    ballSpeedX = 0.0;
    ballSpeedY = 0.0;

    shotFired = false;

    for (int i = 0; i < 256; i++) {
        keyState[i] = false;
        specialKeyState[i] = false;
    }

    player1StealButtonPressed = false;
    player2StealButtonPressed = false;
    resetPlayer(player1, hoopLeftRimX - 28, courtCenterY, false, false);
    resetPlayer(player2, hoopRightRimX - 80, courtCenterY, false, true);
    startTime = time(NULL);
    endGame = false;
}

void calculateAndDrawTime() {
    glColor3fv(colors[WHITE]);
    time_t currentTime = time(NULL);
    double elapsedSeconds = difftime(currentTime, startTime);
    int remainingMinutes = countdownMinutes - 1 - static_cast<int>(elapsedSeconds) / 60;
    int remainingSeconds = 59 - static_cast<int>(elapsedSeconds) % 60;
    if (remainingMinutes == 0 && remainingSeconds == 0) {
        endGame = true;
    }
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
    if (!endGame) {
        glClearColor(0.4609375, 0.4609375, 0.46484375, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        calculateAndDrawTime();
        drawScore();
        drawStaticElements();
        drawBall(ballX, ballY + jump_offset);
        drawPlayer(player1Texture, player1.getPositionX(), player1.getPositionY(), player1.isFlipped());
        drawPlayer(player2Texture, player2.getPositionX(), player2.getPositionY(), player2.isFlipped());
    }

    if (endGame) {
        glClear(GL_COLOR_BUFFER_BIT);
        if (scoreP1 > scoreP2) {
            drawText("Player 1 WON", WINDOW_WIDTH / 2  - 50,WINDOW_HEIGHT / 2);
        }
        else if(scoreP1 < scoreP2) {
            drawText("Player 2 WON", WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2);
        }
        else {
            drawText("ITS A DRAW", WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2);
        }
        drawText("PRESS R TO RESTART", WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 -150);
    }

    glutSwapBuffers();

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

void handleJump(Player& player, float jumpDuration, float jumpHeight) {
    if (player.isJumping()) {
        float elapsedTime = static_cast<float>(clock() - player.getJumpStartTime()) / CLOCKS_PER_SEC;
        if (elapsedTime <= jumpDuration) {
            float t = elapsedTime / jumpDuration;
            float jumpOffset = jumpHeight * 4.0 * t * (1.0 - t);
            float newY = player.getJumpStartY() + jumpOffset;
            player.setPositionY(newY);
        }
        else {
            player.setJumping(false);
        }
    }
}

void handleInput() {
    if (endGame) {
        if (keyState['r'] || keyState['R']) {
            restartGame();
        }
    }
    // Player 1 movement
    for (const auto& movement : player1Movements) {
        for (const auto& key : movement.key) {
            if (keyState[key]) {
                player1.move(movement.deltaX, movement.deltaY);
                player1.setFlipped(movement.flipped);
                break;
            }
        }
    }

    // Player 2 movement
    for (const auto& movement : player2Movements) {
        for (const auto& key : movement.key) {
            if (keyState[key]) {
                player2.move(movement.deltaX, movement.deltaY);
                player2.setFlipped(movement.flipped);
                break;
            }
        }
    }

    // Player 1 shooting
    if (keyState['f'] || keyState['F']) {
        player1.setShootKeyHeld(true);
    }
    // Player 1 shooting release
    if (!keyState['f'] && !keyState['F'] && player1.getShootKeyHeld()) {
        player1.setShootKeyHeld(false);
        if (!shotFired) {
            player1.shootBall(player1.getShootStrength());
        }
    }

    // Player 2 shooting
    if (keyState['p'] || keyState['P']) {
        player2.setShootKeyHeld(true);
    }
    // Player 2 shooting release
    if (!keyState['p'] && !keyState['P'] && player2.getShootKeyHeld()) {
        player2.setShootKeyHeld(false);
        if (!shotFired) {
            player2.shootBall(player2.getShootStrength());
        }
    }

    // Player 1 jumping
    if (keyState[32]) {
        if (!player1.getBallPossesion()) {
            if (!player1.isJumping()) {
                player1.setJumping(true);
                player1.setJumpStartTime(static_cast<float>(clock()));
                player1.setJumpStartY(player1.getPositionY());
            }
        }
    }

    // Player 2 jumping
    if (keyState['['] || keyState['{']) {
        if (!player2.getBallPossesion()) {
            if (!player2.isJumping()) {
                player2.setJumping(true);
                player2.setJumpStartTime(static_cast<float>(clock()));
                player2.setJumpStartY(player2.getPositionY());
            }
        }
    }

    // Player 1 ball stealing
    if (keyState['r'] || keyState['R']) {
        player1StealButtonPressed = true;
    }
    else {
        player1StealButtonPressed = false;
    }

    // Player 2 ball stealing
    if (keyState[';'] || keyState[':']) {
        player2StealButtonPressed = true;
    }
    else {
        player2StealButtonPressed = false;
    }
}

void checkBallCollision() {
    // Check collision with the backboard
    if (ballY >= courtupY - 70 && ballY <= courtupY + 40) {
        if ((ballX >= 50 && ballX <= 70) || (ballX >= (WINDOW_WIDTH - 70) && ballX <= (WINDOW_WIDTH - 50))) {
            ballSpeedX *= -0.5;
            ballSpeedY *= -0.5;
        }
    }

    // Check collision with the rim
    float rimDistanceX = fabs(ballX - hoopLeftRimX);
    float rimDistanceY = fabs(ballY - hoopRimY);
    float distanceToRim = sqrt(pow(rimDistanceX, 2) + pow(rimDistanceY, 2));

    if (distanceToRim <= hoopRimRadius + ballRadius) {
        // Ball collided with the rim
        ballSpeedX *= -0.7;
        ballSpeedY *= -0.7;
    }
}

void screenColision() {
    if (ballX - ballRadius < 0.0 || ballX + ballRadius > WINDOW_WIDTH) {
        ballSpeedX = -ballSpeedX;
    }

    if (ballY - ballRadius < 0.0 || ballY + ballRadius > WINDOW_HEIGHT) {
        ballSpeedY = -ballSpeedY;
    }
}

void resetP1Scored() {
    resetPlayer(player1, (WINDOW_WIDTH / 2) - 80, courtCenterY, false, false);
    resetPlayer(player2, hoopRightRimX - 80, courtCenterY, true, true);
}

void resetP2Scored() {
    resetPlayer(player1, hoopLeftRimX + 5, courtCenterY, true, false);
    resetPlayer(player2, (WINDOW_WIDTH / 2) - 15, courtCenterY, false, true);
}

void checkIfScored() {
    ballInRange = (ballY - ballRadius < hoopRimY + hoopRimRadius) && (ballY + ballRadius > hoopRimY - hoopRimRadius);
    if (ballInRange) {
        float distanceToLeftRim = abs(ballX - hoopLeftRimX);
        if (distanceToLeftRim < hoopRimRadius && !ballPassedThrough) {
            scoreP2 += 1;
            resetP2Scored();
            ballPassedThrough = true;
        }

        float distanceToRightRim = abs(ballX - hoopRightRimX);
        if (distanceToRightRim < hoopRimRadius && !ballPassedThrough) {
            scoreP1 += 1;
            resetP1Scored();
            ballPassedThrough = true;
        }
    }

    if (!ballInRange) {
        ballPassedThrough = false;
    }
}

void dribbleBall() {
    if (player1.getBallPossesion() || player2.getBallPossesion()) {
        jump_offset = jumpAmplitude * sinf(2.0 * M_PI * jumpFrequency * time_ball);
    }
    else {
        jump_offset = 0.0;
    }
}

void handlePlayerBallCollision(float& ballX, float& ballY) {
    touchedP1 = checkPlayerBallCollision(player1.getPositionX(), player1.getPositionY(), ballX, ballY, playerWidth, playerHeight, ballRadius, player1.isFlipped());
    touchedP2 = checkPlayerBallCollision(player2.getPositionX(), player2.getPositionY(), ballX, ballY, playerWidth, playerHeight, ballRadius, player2.isFlipped());
    //printf("P1: %d, P2: %d\n", touchedP1, touchedP2);

    if (!player1.getBallPossesion() && !player2.getBallPossesion() && touchedP1) {
        player1.setBallPossesion(true);
        player2.setBallPossesion(false);
        shotFired = false;
    }
    else if (!player1.getBallPossesion() && !player2.getBallPossesion() && touchedP2) {
        player1.setBallPossesion(false);
        player2.setBallPossesion(true);
        shotFired = false;
    }
    else if (player1.getBallPossesion() && player2StealButtonPressed && touchedP2) {
        player1.setBallPossesion(false);
        player2.setBallPossesion(true);
        shotFired = false;
    }
    else if (player2.getBallPossesion() && player1StealButtonPressed && touchedP1) {
        player1.setBallPossesion(true);
        player2.setBallPossesion(false);
        shotFired = false;  
    }

    if (player1.getBallPossesion()) {
        ballY = player1.getPositionY() + 40;
        ballX = player1.getPositionX() + (player1.isFlipped() ? 50 : 70);
    }
    if (player2.getBallPossesion()) {
        ballY = player2.getPositionY() + 40;
        ballX = player2.getPositionX() + (player2.isFlipped() ? 50 : 70);
    }
}

void handleShooting(float& ballX, float& ballY, float& ballSpeedX, float& ballSpeedY, bool& shotFired) {
    if (shotFired) {
        //printf("ballX: %f, ballY: %f\n", ballX, ballY);
        ballY += ballSpeedY;
        ballX += ballSpeedX;
        ballSpeedY -= gravity;

        if (ballY - ballRadius <= 0.0) {
            ballSpeedX = 0.0f;
            ballSpeedY = 0.0f;
            shotFired = false;
        }
    }
}

void timer(int value) {
    //printf("X:%f, Y:%f\n", player1X, player1Y);
    //printf("Player1 - flipped:%d, jumping: %d, shootKeyHeld: %d, ballPossesion: %d\n", player1.isFlipped(), player1.isJumping(), player1.getShootKeyHeld(), player1.getBallPossesion());
    //printf("Player2 - flipped:%d, jumping: %d, shootKeyHeld: %d, ballPossesion: %d\n", player2.isFlipped(), player2.isJumping(), player2.getShootKeyHeld(), player2.getBallPossesion());
    //printf("ballSpeedX:%f, ballSpeedY:%f\n", ballSpeedX, ballSpeedY);
    //printf("%d", shotFired);
    if(!endGame)
    {
        if (player1.getShootKeyHeld() && !shotFired) {
            player1.setShootStrength(player1.getShootStrength() + 0.035);
        }
        if (player2.getShootKeyHeld() && !shotFired) {
            player2.setShootStrength(player2.getShootStrength() + 0.035);
        }
        handleShooting(ballX, ballY, ballSpeedX, ballSpeedY, shotFired);
        handleJump(player1, jumpDuration, jumpHeight);
        handleJump(player2, jumpDuration, jumpHeight);
        checkBallCollision();
        handlePlayerBallCollision(ballX, ballY);
        screenColision();
        checkIfScored();
        dribbleBall();

        time_ball += TIME_INCREMENT;
    }
    handleInput();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowPosition(10, 60);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    glutCreateWindow("Basketball");
    player1Texture = loadTexture("C:/Users/datcha/source/repos/failaip12/Projekat-Grafika/Projekat/pics/1.png");
    player2Texture = loadTexture("C:/Users/datcha/source/repos/failaip12/Projekat-Grafika/Projekat/pics/2.png");
    if (player1Texture == 0 || player2Texture == 0) {
        return 1;
    }

    startTime = time(NULL);
    initRendering();

    glutKeyboardFunc(myKeyboardFunc);
    glutKeyboardUpFunc(myKeyboardFuncUp);
    glutSpecialFunc(mySpecialKeyFunc);
    glutSpecialUpFunc(mySpecialKeyFuncUp);

    glutReshapeFunc(resizeWindow);

    glutDisplayFunc(drawScene);

    glutTimerFunc(0, timer, 0);

    glutMainLoop();

    return(0);
}