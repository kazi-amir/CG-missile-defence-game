//#include<bits/stdc++.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>


// Variables
int isGameOver = 0;
int isPaused = 0;
int isNight = 0;
float tankX = 400.0;
int muzzleFlash = 0;
int bulletSpawnTimer = 0;
int missileSpawnTimer = 0;
const float missileSpeed = 1.4;
int lives = 5;
int score = 0;

//For Clouds, Stars, Bullets, Missiles and Explosions
const int maxClouds = 6;
typedef struct Cloud{
    float x, y, speed;
}Cloud;
Cloud cloud[maxClouds];

const int maxStars = 50;
typedef struct Star{
    float x, y, size, brightness;
}Star;
Star star[maxStars];

const int maxBullets = 30;
typedef struct Bullet{
    float x, y;
    int isActive;
}Bullet;
Bullet bullet[maxBullets];

const int maxMissiles = 20;
typedef struct Missile{
    float x, y;
    int isActive;
}Missile;
Missile missile[maxMissiles];

const int maxExplosions = 20;
typedef struct Explosion{
    float x, y;
    int isActive, lifeSpan;
}Explosion;
Explosion explosion[maxExplosions];


void spawnExplosion(float ex, float ey);

//BRESENHAM'S LINE ALGORITHM
void drawLineBresenham(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    glBegin(GL_POINTS);
    while (true) {
        glVertex2i(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
    glEnd();
}
//Mid Point Circle
void drawFilledCircle(int centerX, int centerY, int radius) {
    int x = radius;
    int y = 0;
    int p = 1 - radius;

    glBegin(GL_POINTS);
    while (x >= y) {
        //Fill the circle by drawing lines
        for (int i = centerX - x; i <= centerX + x; i++) {
            glVertex2i(i, centerY + y);
            glVertex2i(i, centerY - y);
        }
        for (int i = centerX - y; i <= centerX + y; i++) {
            glVertex2i(i, centerY + x);
            glVertex2i(i, centerY - x);
        }
        y++;
        if (p <= 0) {
            p = p + 2 * y + 1;
        } else {
            x--;
            p = p + 2 * y - 2 * x + 1;
        }
    }
    glEnd();
}

//SUN
void drawSun() {
    int cx = 800, cy = 500, r = 28;
    //circle
    glColor3f(1.0f, 1.0f, 0.0f);
    drawFilledCircle(cx, cy, r);

    // Sun Rays using Bresenham
    glColor3f(1.0f, 0.8f, 0.0f);

    // Straight rays
    drawLineBresenham(cx, cy + 35, cx, cy + 65); // Top
    drawLineBresenham(cx, cy - 35, cx, cy - 65); // Bottom
    drawLineBresenham(cx + 35, cy, cx + 65, cy); // Right
    drawLineBresenham(cx - 35, cy, cx - 65, cy); // Left
    // Diagonal rays
    drawLineBresenham(cx + 25, cy + 25, cx + 50, cy + 50);
    drawLineBresenham(cx - 25, cy + 25, cx - 50, cy + 50);
    drawLineBresenham(cx + 25, cy - 25, cx + 50, cy - 50);
    drawLineBresenham(cx - 25, cy - 25, cx - 50, cy - 50);
}

//MOON
void drawMoon() {
    int cx = 760, cy = 500, r = 25;

    // Yellow moon circle
    glColor3f(0.99f, 1.0f, 0.85f);
    drawFilledCircle(cx, cy, r);

    // Crescent effect (dark overlay circle)
    glColor3f(0.01f, 0.05f, 0.15f);
    drawFilledCircle(cx + r * 0.5f, cy + 10, r * 0.9f);
}

//CLOUD
void drawCloud(int x, int y) {
    if(isNight) {
        glColor3f(0.5f, 0.5f, 0.5f); // Dim Gray
    } else {
        glColor3f(0.9f, 0.9f, 0.9f); // White
    }

    drawFilledCircle(x, y, 20);         // Left part
    drawFilledCircle(x + 25, y + 8, 30); // Center (bigger and higher)
    drawFilledCircle(x + 50, y, 20);     // Right part
}
void initClouds(){
    for(int i = 0; i < maxClouds; i++){
        cloud[i].x = -70 + rand() % 500;
        cloud[i].y = 400 + rand() % 120;
        cloud[i].speed = 0.3 + (rand() % 10) / 20.0;
    }
}

void drawClouds() {
    for (int i = 0; i < maxClouds; i++){
        drawCloud(cloud[i].x, cloud[i].y);
    }
}

void updateClouds(){
    for(int i = 0; i < maxClouds; i++){
        cloud[i].x += cloud[i].speed;
        if(cloud[i].x > 960){
            cloud[i].x = -70;
            cloud[i].y = 400 + rand() % 160;
        }
    }
}

//STARS
// 100 stars position and size and brightness randomize
void initStars(){
    for(int i = 0; i < maxStars; i++){
        star[i].x = 5 + rand() % 795;
        star[i].y = 300 + rand() % 290;
        star[i].size = 0.5 + (rand() % 2) / 2.0;
        star[i].brightness = 0.5 + (rand() % 30) / 100.0;
    }
}

//Draw stars
void drawStars(){
    for(int i = 0; i< maxStars; i++){
        glColor3f(0.83 * star[i].brightness, 0.68 * star[i].brightness, 0.21 * star[i].brightness);
        drawFilledCircle(star[i].x,star[i].y, star[i].size);
    }
}

//LEFT TREE
void drawLeftTree(int x, int y) {
    //SHADOW TRUNK
    glColor3f(0.35f, 0.2f, 0.1f); //dark brown
    glBegin(GL_POLYGON);
        glVertex2d(x - 12, y - 20); // y-20 for starting from grass line
        glVertex2d(x + 12, y - 20);
        glVertex2d(x + 10, y + 100);
        glVertex2d(x - 10, y + 100);
    glEnd();

    //MAIN TRUNK
    glColor3f(0.54f, 0.27f, 0.07f);//normal brown
    glBegin(GL_POLYGON);
        glVertex2d(x - 8, y - 20);
        glVertex2d(x + 8, y - 20);
        glVertex2d(x + 6, y + 100);
        glVertex2d(x - 6, y + 100);
    glEnd();

    //LOWER leaf
    glColor3f(0.0f, 0.35f, 0.0f); //dark green
    drawFilledCircle(x - 40, y + 110, 35);
    drawFilledCircle(x + 40, y + 110, 35);
    drawFilledCircle(x, y + 100, 45);

    // UPPER leaf
    glColor3f(0.0f, 0.5f, 0.0f); // green
    drawFilledCircle(x - 25, y + 140, 35);
    drawFilledCircle(x + 25, y + 140, 35);
    drawFilledCircle(x, y + 165, 40);

    // HIGHLIGHTS
    glColor3f(0.1f, 0.7f, 0.1f); // light green
    drawFilledCircle(x - 15, y + 170, 15);
    drawFilledCircle(x + 15, y + 150, 20);
}

//RIGHT TREE
void drawRightTree(int x, int y) {
    //shadow trunk
    glColor3f(0.35f, 0.2f, 0.1f);//dark brown
    glBegin(GL_POLYGON);
        glVertex2d(x - 12, y );
        glVertex2d(x + 12, y );
        glVertex2d(x + 10, y + 100);
        glVertex2d(x - 10, y + 100);
    glEnd();

    //MAIN TRUNK
    glColor3f(0.54f, 0.27f, 0.07f);// normal brown
    glBegin(GL_POLYGON);
        glVertex2d(x - 8, y );
        glVertex2d(x + 8, y );
        glVertex2d(x + 6, y + 100);
        glVertex2d(x - 6, y + 100);
    glEnd();

    //Main Branches
    glBegin(GL_TRIANGLES);
        glVertex2d(x, y + 80); glVertex2d(x - 30, y + 130); glVertex2d(x - 20, y + 135);
        glVertex2d(x, y + 80); glVertex2d(x + 30, y + 130); glVertex2d(x + 20, y + 135);
    glEnd();


    // Shadow leaf Layer
    glColor3f(0.0f, 0.25f, 0.0f); // dark green
    drawFilledCircle(x - 35, y + 140, 45);
    drawFilledCircle(x + 35, y + 140, 45);
    drawFilledCircle(x, y + 170, 50);

    // Highlight leaf Layer
    glColor3f(0.0f, 0.55f, 0.0f); // green
    drawFilledCircle(x - 20, y + 160, 35);
    drawFilledCircle(x + 10, y + 185, 30);

    // Sunlight Spots
    glColor3f(0.2f, 0.7f, 0.2f); // light green
    drawFilledCircle(x - 5, y + 195, 15);
}

//Small Tree
void drawSmallTree(int x, int y) {
    //Trunk
    glColor3f(0.4f, 0.2f, 0.1f);
    glBegin(GL_POLYGON);
        glVertex2d(x - 6, y);
        glVertex2d(x + 6, y);
        glVertex2d(x + 4, y + 50);
        glVertex2d(x - 4, y + 50);
    glEnd();

    //Simple Branch
    glBegin(GL_TRIANGLES);
        glVertex2d(x, y + 40); glVertex2d(x - 15, y + 65); glVertex2d(x - 10, y + 68);
        glVertex2d(x, y + 40); glVertex2d(x + 15, y + 65); glVertex2d(x + 10, y + 68);
    glEnd();

    // Shadow leaf
    glColor3f(0.0f, 0.3f, 0.0f);// dark green
    drawFilledCircle(x - 15, y + 70, 20);
    drawFilledCircle(x + 15, y + 70, 20);
    drawFilledCircle(x, y + 85, 25);

    // Highlight leaf
    glColor3f(0.0f, 0.6f, 0.0f);//light green
    drawFilledCircle(x - 8, y + 80, 15);
    drawFilledCircle(x + 5, y + 90, 12);
}

// Bushes above grass
void drawBigBushes(int x, int y) {

    //Shadow Layer
    glColor3f(0.0f, 0.2f, 0.05f);// dark green
    drawFilledCircle(x - 10, y + 15, 35);      // Left lobe
    drawFilledCircle(x + 55, y + 15, 35);      // Right lobe
    drawFilledCircle(x + 22, y + 35, 45);      // Top center

    //Mid-Tone Layer
    glColor3f(0.0f, 0.4f, 0.1f);//forest green
    drawFilledCircle(x, y + 20, 25);
    drawFilledCircle(x + 45, y + 20, 25);
    drawFilledCircle(x + 22, y + 40, 30);

    //Highlight Layer
    glColor3f(0.2f, 0.65f, 0.2f);//lime green
    drawFilledCircle(x + 10, y + 35, 15);
    drawFilledCircle(x + 22, y + 55, 18);

    //Flower
    glColor3f(1.0f, 0.3f, 0.3f);
    drawFilledCircle(x + 5, y + 25, 3);
    drawFilledCircle(x + 40, y + 45, 3);
    drawFilledCircle(x + 20, y + 15, 3);
    drawFilledCircle(x + 50, y + 30, 3);
}


//ROAD
void road() {
    if (isNight){
        glColor3f(0.1f, 0.1f, 0.15f);
    } else {
        glColor3f(0.3f, 0.3f, 0.3f); //Grey
    }

    glBegin(GL_POLYGON);
        glVertex2d(0, 0);       // Bottom Left
        glVertex2d(900, 0);     // Bottom Right
        glVertex2d(900, 100);   // Top Right
        glVertex2d(0, 100);     // Top Left
    glEnd();
}

//ROAD BORDER
void roadBorder(){
    glColor3f(1.0f, 1.0f, 1.0f); // white road border
    glBegin(GL_POLYGON);
        glVertex2d(0, 100);       // Bottom Left
        glVertex2d(900, 100);     // Bottom Right
        glVertex2d(900, 110);   // Top Right
        glVertex2d(0, 110);     // Top Left
    glEnd();
}

//ROAD MARKERS
void roadMarkers(){
    glColor3f(1.0f, 1.0f, 1.0f); // White color

    // Marker 1
    glBegin(GL_POLYGON);
        glVertex2d(50, 45); glVertex2d(150, 45); glVertex2d(150, 55); glVertex2d(50, 55);
    glEnd();

    // Marker 2
    glBegin(GL_POLYGON);
        glVertex2d(240, 45); glVertex2d(340, 45); glVertex2d(340, 55); glVertex2d(240, 55);
    glEnd();

    // Marker 3
    glBegin(GL_POLYGON);
        glVertex2d(440, 45); glVertex2d(540, 45); glVertex2d(540, 55); glVertex2d(440, 55);
    glEnd();

    // Marker 4
    glBegin(GL_POLYGON);
        glVertex2d(650, 45); glVertex2d(750, 45); glVertex2d(750, 55); glVertex2d(650, 55);
    glEnd();

    // Marker 5
    glBegin(GL_POLYGON);
        glVertex2d(840, 45); glVertex2d(900, 45); glVertex2d(900, 55); glVertex2d(840, 55);
    glEnd();

}

//GRASS
void grass(){
    if(isNight) {
            glColor3f(0.05,0.3,0.05);
    } else{
        glColor3f(0.13f, 0.55f, 0.13f); // forest green grass
    }

    glBegin(GL_POLYGON);
        glVertex2d(0, 110);       // Bottom Left
        glVertex2d(900, 110);     // Bottom Right
        glVertex2d(900, 200);   // Top Right
        glVertex2d(0, 200);     // Top Left
    glEnd();
}

//SKY
void sky(){
    if (isNight) {
        // Night Gradient
        glBegin(GL_QUADS);
            glColor3f(0.02f, 0.05f, 0.15f); glVertex2d(0, 600); glVertex2d(900, 600);
            glColor3f(0.1f, 0.1f, 0.3f);    glVertex2d(900, 200); glVertex2d(0, 200);
        glEnd();
    } else {
        glColor3f(0.53f, 0.81f, 0.98f);
    glBegin(GL_POLYGON);
        glVertex2d(0, 200);       // Bottom Left
        glVertex2d(900, 200);     // Bottom Right
        glVertex2d(900, 600);   // Top Right
        glVertex2d(0, 600);     // Top Left
    glEnd();
    }
}


//BUILDINGS
void drawBuildings(){
    //Left Building
    if (isNight) {
        glColor3f(0.6f, 0.6f, 0.6f);
    } else{
        glColor3f(0.7f, 0.7f, 0.7f);// Light Grey
    }

    glBegin(GL_POLYGON);
        glVertex2d(300, 200);   // Bottom Left
        glVertex2d(400, 200);   // Bottom Right
        glVertex2d(400, 350 );   // Top Right
        glVertex2d(300, 350);   // Top Left
    glEnd();

    if (isNight){
        glColor3f(0.9f, 0.9f, 0.0f); // Dim Yellow
    } else {
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    // Top Row
    glBegin(GL_POLYGON); glVertex2d(310, 300); glVertex2d(340, 300); glVertex2d(340, 330); glVertex2d(310, 330); glEnd();
    glBegin(GL_POLYGON); glVertex2d(360, 300); glVertex2d(390, 300); glVertex2d(390, 330); glVertex2d(360, 330); glEnd();
    // Row 2
    glBegin(GL_POLYGON); glVertex2d(310, 260); glVertex2d(340, 260); glVertex2d(340, 290); glVertex2d(310, 290); glEnd();
    glBegin(GL_POLYGON); glVertex2d(360, 260); glVertex2d(390, 260); glVertex2d(390, 290); glVertex2d(360, 290); glEnd();
    // Bottom Row
    glBegin(GL_POLYGON); glVertex2d(310, 220); glVertex2d(340, 220); glVertex2d(340, 250); glVertex2d(310, 250); glEnd();
    glBegin(GL_POLYGON); glVertex2d(360, 220); glVertex2d(390, 220); glVertex2d(390, 250); glVertex2d(360, 250); glEnd();

    //Middle Building
    if (isNight) {
        glColor3f(0.25f, 0.15f, 0.08f);
    } else {
    glColor3f(0.6f, 0.4f, 0.2f); // Brown
    }

    glBegin(GL_POLYGON);
        glVertex2d(420, 200);   // Bottom Left
        glVertex2d(520, 200);   // Bottom Right
        glVertex2d(520, 380);   // Top Right
        glVertex2d(420, 380);   // Top Left
    glEnd();

    if (isNight){
        glColor3f(0.9f, 0.9f, 0.0f); // Dim Yellow
    } else {
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    // Row 1 (Top)
    glBegin(GL_POLYGON); glVertex2d(430, 340); glVertex2d(460, 340); glVertex2d(460, 370); glVertex2d(430, 370); glEnd();
    glBegin(GL_POLYGON); glVertex2d(480, 340); glVertex2d(510, 340); glVertex2d(510, 370); glVertex2d(480, 370); glEnd();

    // Row 2
    glBegin(GL_POLYGON); glVertex2d(430, 300); glVertex2d(460, 300); glVertex2d(460, 330); glVertex2d(430, 330); glEnd();
    glBegin(GL_POLYGON); glVertex2d(480, 300); glVertex2d(510, 300); glVertex2d(510, 330); glVertex2d(480, 330); glEnd();

    // Row 3
    glBegin(GL_POLYGON); glVertex2d(430, 260); glVertex2d(460, 260); glVertex2d(460, 290); glVertex2d(430, 290); glEnd();
    glBegin(GL_POLYGON); glVertex2d(480, 260); glVertex2d(510, 260); glVertex2d(510, 290); glVertex2d(480, 290); glEnd();

    // Row 4 (Bottom)
    glBegin(GL_POLYGON); glVertex2d(430, 220); glVertex2d(460, 220); glVertex2d(460, 250); glVertex2d(430, 250); glEnd();
    glBegin(GL_POLYGON); glVertex2d(480, 220); glVertex2d(510, 220); glVertex2d(510, 250); glVertex2d(480, 250); glEnd();

    //Right Building
    if (isNight) {
        glColor3f(0.6f, 0.6f, 0.6f);
    } else{
        glColor3f(0.7f, 0.7f, 0.7f);// Light Grey
    }

    glBegin(GL_POLYGON);
        glVertex2d(540, 200);   // Bottom Left
        glVertex2d(640, 200);   // Bottom Right
        glVertex2d(640, 330);   // Top Right
        glVertex2d(540, 330);   // Top Left
    glEnd();

    if (isNight){
        glColor3f(0.9f, 0.9f, 0.0f); // Dim Yellow
    } else {
        glColor3f(1.0f, 1.0f, 1.0f); //White
    }

    // Row 1 (Top)
    glBegin(GL_POLYGON); glVertex2d(550, 285); glVertex2d(580, 285); glVertex2d(580, 315); glVertex2d(550, 315); glEnd();
    glBegin(GL_POLYGON); glVertex2d(600, 285); glVertex2d(630, 285); glVertex2d(630, 315); glVertex2d(600, 315); glEnd();

    // Row 2 (Middle)
    glBegin(GL_POLYGON); glVertex2d(550, 245); glVertex2d(580, 245); glVertex2d(580, 275); glVertex2d(550, 275); glEnd();
    glBegin(GL_POLYGON); glVertex2d(600, 245); glVertex2d(630, 245); glVertex2d(630, 275); glVertex2d(600, 275); glEnd();

    // Row 3 (Bottom)
    glBegin(GL_POLYGON); glVertex2d(550, 205); glVertex2d(580, 205); glVertex2d(580, 235); glVertex2d(550, 235); glEnd();
    glBegin(GL_POLYGON); glVertex2d(600, 205); glVertex2d(630, 205); glVertex2d(630, 235); glVertex2d(600, 235); glEnd();

}

//Roadside Bushes
void drawBushes(int x, int y) {
    //Shadow
    if (isNight) {
        glColor3f(0.0f, 0.4f, 0.2f); // Slightly brighter
    } else {
        glColor3f(0.0f, 0.3f, 0.0f);//dark green
    }
    drawFilledCircle(x, y + 15, 20);      // Left
    drawFilledCircle(x + 40, y + 15, 20); // Right
    drawFilledCircle(x + 20, y + 25, 25); // center

    //Highlight
    glColor3f(0.1f, 0.5f, 0.1f); //light green
    drawFilledCircle(x + 5, y + 20, 12);
    drawFilledCircle(x + 35, y + 20, 12);
    drawFilledCircle(x + 20, y + 30, 15);
}

//MOUNTAINS
void mountains(){
//  MOUNTAIN 1
    if (isNight) {
        glColor3f(0.02f, 0.05f, 0.15f);
    } else {
        glColor3f(0.2f, 0.3f, 0.4f);// Dark blue gray
    }
    glBegin(GL_TRIANGLES);
        glVertex2d(100, 200); glVertex2d(350, 450); glVertex2d(600, 200);
    glEnd();

    //  MOUNTAIN 2
    if (isNight) {
            glColor3f(0.1f, 0.2f, 0.25f);
    } else {
        glColor3f(0.3f, 0.4f, 0.5f); // lighter gray blue
    }
    glBegin(GL_TRIANGLES);
        glVertex2d(500, 200); glVertex2d(750, 400); glVertex2d(950, 200);
    glEnd();

}

//Missile Defender
void drawTank(int x, int y) {
    // The foundation
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_POLYGON);
        glVertex2d(x + 15, y + 5); glVertex2d(x + 105, y + 5);
        glVertex2d(x + 105, y + 25); glVertex2d(x + 15, y + 25);
    glEnd();

    // Border for Track Base
    glColor3f(0.0f, 0.0f, 0.0f);
    drawLineBresenham(x + 15, y + 5, x + 105, y + 5);
    drawLineBresenham(x + 105, y + 5, x + 105, y + 25);
    drawLineBresenham(x + 105, y + 25, x + 15, y + 25);
    drawLineBresenham(x + 15, y + 25, x + 15, y + 5);

    // Wheel System
    for (int i = 0; i < 4; i++) {
        int wx = x + 25 + (i * 22);
        glColor3f(0.0f, 0.0f, 0.0f); // Tire
        drawFilledCircle(wx, y + 12, 11);
        glColor3f(0.4f, 0.4f, 0.4f); // Hubcap detail
        drawFilledCircle(wx, y + 12, 4);
    }

    //Armor
    glColor3f(0.05f, 0.25f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2d(x + 5, y + 25); glVertex2d(x + 115, y + 25);
        glVertex2d(x + 105, y + 55); glVertex2d(x + 15, y + 55);
    glEnd();

    // Border for Armor
    glColor3f(0.0f, 0.0f, 0.0f);
    drawLineBresenham(x + 5, y + 25, x + 115, y + 25);
    drawLineBresenham(x + 115, y + 25, x + 105, y + 55);
    drawLineBresenham(x + 105, y + 55, x + 15, y + 55);
    drawLineBresenham(x + 15, y + 55, x + 5, y + 25);

    // 4. Main Turret
    glColor3f(0.1f, 0.4f, 0.1f);
    glBegin(GL_POLYGON);
        glVertex2d(x + 30, y + 55); glVertex2d(x + 90, y + 55);
        glVertex2d(x + 85, y + 80); glVertex2d(x + 35, y + 80);
    glEnd();

    // Border for Turret
    glColor3f(0.0f, 0.0f, 0.0f);
    drawLineBresenham(x + 30, y + 55, x + 90, y + 55);
    drawLineBresenham(x + 90, y + 55, x + 85, y + 80);
    drawLineBresenham(x + 85, y + 80, x + 35, y + 80);
    drawLineBresenham(x + 35, y + 80, x + 30, y + 55);

    // 5. Cannon Barrel
    glColor3f(0.3f, 0.4f, 0.1f);
    glBegin(GL_POLYGON);
        glVertex2d(x + 55, y + 80); glVertex2d(x + 65, y + 80);
        glVertex2d(x + 63, y + 120); glVertex2d(x + 57, y + 120);
    glEnd();

    // Border for Barrel
    glColor3f(0.0f, 0.0f, 0.0f);
    drawLineBresenham(x + 55, y + 80, x + 65, y + 80);
    drawLineBresenham(x + 65, y + 80, x + 63, y + 120);
    drawLineBresenham(x + 63, y + 120, x + 57, y + 120);
    drawLineBresenham(x + 57, y + 120, x + 55, y + 80);

    // 6. Muzzle Brake (The tip)
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_POLYGON);
        glVertex2d(x + 54, y + 120); glVertex2d(x + 66, y + 120);
        glVertex2d(x + 66, y + 130); glVertex2d(x + 54, y + 130);
    glEnd();

    // Border for Muzzle
    glColor3f(0.0f, 0.0f, 0.0f);
    drawLineBresenham(x + 54, y + 120, x + 66, y + 120);
    drawLineBresenham(x + 66, y + 120, x + 66, y + 130);
    drawLineBresenham(x + 66, y + 130, x + 54, y + 130);
    drawLineBresenham(x + 54, y + 130, x + 54, y + 120);
}

//Bench
void drawBench(int x, int y) {
    //LEGS Using Bresenham
    glColor3f(0.1f, 0.1f, 0.1f); // Black/Dark Grey

    // Left Leg
    drawLineBresenham(x, y, x, y + 15);
    drawLineBresenham(x + 5, y, x + 5, y + 15);
    // Right Leg
    drawLineBresenham(x + 55, y, x + 55, y + 15);
    drawLineBresenham(x + 60, y, x + 60, y + 15);

    //SEAT
    glColor3f(0.5f, 0.35f, 0.2f); // Wood brown
    glBegin(GL_POLYGON);
        glVertex2d(x - 5, y + 15);
        glVertex2d(x + 65, y + 15);
        glVertex2d(x + 65, y + 22);
        glVertex2d(x - 5, y + 22);
    glEnd();

    //Back with bresenham
    glColor3f(0.1f, 0.1f, 0.1f);
    drawLineBresenham(x + 5, y + 22, x + 5, y + 45);
    drawLineBresenham(x + 55, y + 22, x + 55, y + 45);

    // 4. BACKREST (Top Wood Plank)
    glColor3f(0.6f, 0.4f, 0.25f); // Slightly lighter wood
    glBegin(GL_POLYGON);
        glVertex2d(x - 5, y + 35);
        glVertex2d(x + 65, y + 35);
        glVertex2d(x + 65, y + 45);
        glVertex2d(x - 5, y + 45);
    glEnd();

    // Borders for wood
    glColor3f(0.0f, 0.0f, 0.0f);
    drawLineBresenham(x - 5, y + 15, x + 65, y + 15); // Seat border
    drawLineBresenham(x - 5, y + 35, x + 65, y + 35); // Backrest border
}
void drawLamp(int x, int y) {
    //Lamp Post
    glColor3f(0.2f, 0.2f, 0.2f); // Dark Grey
    for(int i = -2; i <= 2; i++) {
        drawLineBresenham(x + i, y, x + i, y + 150);
    }

    //Horizontal Arm
    drawLineBresenham(x, y + 150, x + 30, y + 150);

    // The Lamp Head
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_POLYGON);
        glVertex2d(x + 20, y + 150);
        glVertex2d(x + 40, y + 150);
        glVertex2d(x + 45, y + 140);
        glVertex2d(x + 15, y + 140);
    glEnd();

    //The Light Bulb (Glows at night)
    if (isNight) {
        // Outer Glow
        glColor4f(1.0f, 0.9f, 0.0f, 0.3f); // Transparent yellow
        drawFilledCircle(x + 30, y + 135, 15);

        // Bright Bulb
        glColor3f(1.0f, 1.0f, 0.8f); // Warm white
        drawFilledCircle(x + 30, y + 135, 8);
    } else {
        glColor3f(0.4f, 0.4f, 0.4f); // Off (Grey)
        drawFilledCircle(x + 30, y + 135, 8);
    }
}

//Bullets from Tank
void initBullets() {
    for (int i = 0; i < maxBullets; i++){
        bullet[i].isActive = 0;
    }
}
void spawnBullet() {
    for (int i = 0; i < maxBullets; i++) {
        if (!bullet[i].isActive) {
            bullet[i].x = tankX + 60; // Center of barrel
            bullet[i].y = 170;   // Barrel tip Y position
            bullet[i].isActive = 1;
            muzzleFlash = 5;         // Flash for 5 frames
            break;
        }
    }
}
void drawOneBullet(float bx, float by) {
    // Bullet body (red - yellow)
    glColor3f(1.0, 0.5, 0.0);
    glBegin(GL_QUADS);
        glVertex2f(bx - 2, by);
        glVertex2f(bx + 2, by);
        glVertex2f(bx + 2, by + 15);
        glVertex2f(bx - 2, by + 15);
    glEnd();

    // Bright tip
    glColor3f(1.0, 1.0, 0.6);
    glBegin(GL_QUADS);
        glVertex2f(bx - 2, by + 12);
        glVertex2f(bx + 2, by + 12);
        glVertex2f(bx + 2, by + 15);
        glVertex2f(bx - 2, by + 15);
    glEnd();
}
void drawAllBullets() {
    for (int i = 0; i < maxBullets; i++){
        if (bullet[i].isActive){
            drawOneBullet(bullet[i].x, bullet[i].y);
        }
    }
}
void updateBullets() {
    for (int i = 0; i < maxBullets; i++) {
        if (bullet[i].isActive) {
            bullet[i].y += 7;              // Move up
            if (bullet[i].y > 620){           // Off screen
                bullet[i].isActive = 0;
            }
        }
    }
}

//MISSILES
void initMissiles(){
    for(int i = 0; i < maxMissiles; i++){
        missile[i].isActive = 0;
    }
}
void spawnMissile() {
    for (int i = 0; i < maxMissiles; i++) {
        if (!missile[i].isActive) {
            missile[i].x = 50 + (rand() % 850);  // Random X within play area
            missile[i].y = 600;                  // Start at top
            missile[i].isActive = 1;
            break;
        }
    }
}
void drawOneMissile(float mx, float my) {
    // Main body (red)
    glColor3f(0.85, 0.15, 0.0);
    glBegin(GL_QUADS);
        glVertex2f(mx - 4, my);
        glVertex2f(mx + 4, my);
        glVertex2f(mx + 4, my + 30);
        glVertex2f(mx - 4, my + 30);
    glEnd();

    // Dark bottom section
    glColor3f(0.65, 0.05, 0.0);
    glBegin(GL_QUADS);
        glVertex2f(mx - 4, my);
        glVertex2f(mx + 4, my);
        glVertex2f(mx + 4, my + 9);
        glVertex2f(mx - 4, my + 9);
    glEnd();

    // Pointy Head
    glColor3f(1.0, 0.35, 0.0);
    glBegin(GL_TRIANGLES);
        glVertex2f(mx, my - 8);      // Tip
        glVertex2f(mx - 4, my);      // Left base
        glVertex2f(mx + 4, my);      // Right base
    glEnd();

    // Stripe detail
    glColor3f(1.0, 0.45, 0.1);
    glBegin(GL_QUADS);
        glVertex2f(mx - 4, my + 12);
        glVertex2f(mx + 4, my + 12);
        glVertex2f(mx + 4, my + 15);
        glVertex2f(mx - 4, my + 15);
    glEnd();

    // Left fin
    glColor3f(0.75, 0.1, 0.0);
    glBegin(GL_TRIANGLES);
        glVertex2f(mx - 4, my + 24);
        glVertex2f(mx - 11, my + 36);
        glVertex2f(mx - 4, my + 30);
    glEnd();

    // Right fin
    glBegin(GL_TRIANGLES);
        glVertex2f(mx + 4, my + 24);
        glVertex2f(mx + 11, my + 36);
        glVertex2f(mx + 4, my + 30);
    glEnd();

    // Top fin (small)
    glBegin(GL_TRIANGLES);
        glVertex2f(mx - 1, my + 30);
        glVertex2f(mx + 1, my + 30);
        glVertex2f(mx, my + 39);
    glEnd();
}

void drawAllMissiles() {
    for(int i = 0; i < maxMissiles; i++){
        if(missile[i].isActive){
            drawOneMissile(missile[i].x, missile[i].y);
        }
    }
}
void updateMissiles() {
    for (int i = 0; i < maxMissiles; i++) {
        if (missile[i].isActive) {
            missile[i].y -= missileSpeed;    // Fall down
            if (missile[i].y < 71) {         // Hit the road
               spawnExplosion(missile[i].x, 71.0);
                missile[i].isActive = 0;
                lives--;
                if (lives <= 0) {
                    lives = 0;
                    isGameOver = 1;
                }
            }
        }
    }
}

//EXPLOSIONS WHEN BULLET HITS A MISSILE
void initExplosions(){
    for(int i = 0; i < maxExplosions; i++){
        explosion[i].isActive = 0;
        explosion[i].lifeSpan = 0;
    }
}
void spawnExplosion(float ex, float ey){
    for(int i = 0; i < maxExplosions; i++){
        if(!explosion[i].isActive){
            explosion[i].x = ex;
            explosion[i].y = ey;
            explosion[i].lifeSpan = 20;
            explosion[i].isActive = 1;
            break;
        }
    }
}
void drawAllExplosion() {
    for (int i = 0; i < maxExplosions; i++) {
        if (!explosion[i].isActive) continue;

        float t = explosion[i].lifeSpan / 20.0;    // 1.0 → 0.0 as it ages
        int size = (int)(10 + (1.0 - t) * 16);     // Grows from 10 to 26

        // Outer fire (red-orange, fades)
        glColor3f(1.0, 0.3 * t, 0.0);
        drawFilledCircle(explosion[i].x, explosion[i].y, size);

        // Inner core (bright yellow, shrinks)
        glColor3f(1.0, 1.0 * t, 0.2 * t);
        drawFilledCircle(explosion[i].x, explosion[i].y, size / 2);
    }
}
void updateExplosions() {
    for (int i = 0; i < maxExplosions; i++) {
        if (explosion[i].isActive) {
            explosion[i].lifeSpan--;
            if (explosion[i].lifeSpan <= 0){
                explosion[i].isActive = 0;
            }
        }
    }
}

//COLLISION OF BULLETS AND MISSILES
void checkColission(){
    for(int i = 0; i < maxBullets; i++){
        if(!bullet[i].isActive) continue;

        for(int j = 0; j < maxMissiles; j++){
            if(!missile[j].isActive) continue;

            //calculate X distance and Y distance to check if collides
            float dx = bullet[i].x - missile[j].x;
            float dy = bullet[i].y - missile[j].y + 15;

            float dist = sqrt((dx * dx) + (dy * dy));

            if(dist < 22){
                //Hits bullet
                spawnExplosion(missile[j].x, missile[j].y + 15);
                bullet[i].isActive = 0;
                missile[j].isActive = 0;
                score++;
                break;
            }
        }
    }
}

//String Drawing Function
void drawString(float x, float y, const char* str, void* font) {
    glRasterPos2f(x, y);
    while (*str) {
        glutBitmapCharacter(font, *str);
        str++;
    }
}

//Draw UHD 
void drawHUD() {
    char scoreStr[50];
    char livesStr[50];
    sprintf(scoreStr, "Score: %d", score);

    // Score (top-left)
    glColor3f(1.0, 1.0, 1.0);
    drawString(20, 570, scoreStr, GLUT_BITMAP_HELVETICA_18);

    // Day/Night label
    if (isNight) {
        glColor3f(0.6, 0.6, 0.9);
        drawString(20, 550, "N: Day/Night", GLUT_BITMAP_HELVETICA_12);
    } else {
        glColor3f(0.0, 0.2, 0.6);
        drawString(20, 550, "N: Day/Night", GLUT_BITMAP_HELVETICA_12);
    }

    // Controls hint
    if(isNight){
        glColor3f(0.9, 0.9, 0.9);
    } 
    else {
        glColor3f(0.1, 0.1, 0.1);
    }
    drawString(20, 530, "A/D or Arrows: Move ", GLUT_BITMAP_HELVETICA_12);
    drawString(20, 510, "Space: Manual Shoot", GLUT_BITMAP_HELVETICA_12);
    drawString(20, 490, "P: Pause/Resume", GLUT_BITMAP_HELVETICA_12);
    drawString(20, 470, "R: Restart", GLUT_BITMAP_HELVETICA_12);
    drawString(20, 450, "ESC: Quit", GLUT_BITMAP_HELVETICA_12);

    // Lives label (top-right)
    glColor3f(1.0, 1.0, 1.0);
    drawString(700, 570, "Lives:", GLUT_BITMAP_HELVETICA_18);

    // Hearts for each life
    for (int i = 0; i < lives; i++) {
        float hx = 770 + i * 24;
        float hy = 575;

        // Heart = two circles + downward triangle
        glColor3f(1.0, 0.15, 0.15);
        drawFilledCircle(hx + 4, hy, 5);     // Left bump
        drawFilledCircle(hx + 12, hy, 5);    // Right bump

        glColor3f(1.0, 0.15, 0.15);
        glBegin(GL_TRIANGLES);
            glVertex2f(hx, hy - 2);
            glVertex2f(hx + 16, hy - 2);
            glVertex2f(hx + 8, hy - 11);     // Bottom point
        glEnd();
    }

    // Pause overlay
    if (isPaused && !isGameOver) {
        // Semi-dark backdrop
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_QUADS);
            glVertex2f(300, 270);
            glVertex2f(600, 270);
            glVertex2f(600, 340);
            glVertex2f(300, 340);
        glEnd();

        // Border using Bresenham
        glColor3f(1.0, 1.0, 0.0);
        drawLineBresenham(300, 270, 600, 270);
        drawLineBresenham(600, 270, 600, 340);
        drawLineBresenham(600, 340, 300, 340);
        drawLineBresenham(300, 340, 300, 270);

        glColor3f(1.0, 1.0, 0.0);
        drawString(405, 310, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(0.8, 0.8, 0.8);
        drawString(400, 285, "Press P to resume", GLUT_BITMAP_HELVETICA_12);
    }
}

//GameOver
void drawGameOver() {
    char scoreStr[60];

    sprintf(scoreStr, "Final Score: %d", score);

    // Dark panel background
    glColor3f(0.0, 0.0, 0.15);
    glBegin(GL_QUADS);
        glVertex2f(250, 180);
        glVertex2f(650, 180);
        glVertex2f(650, 420);
        glVertex2f(250, 420);
    glEnd();

    // Red border using Bresenham
    glColor3f(1.0, 0.2, 0.2);
    drawLineBresenham(250, 180, 650, 180);
    drawLineBresenham(650, 180, 650, 420);
    drawLineBresenham(650, 420, 250, 420);
    drawLineBresenham(250, 420, 250, 180);

    // Inner border (double line effect)
    glColor3f(0.8, 0.15, 0.15);
    drawLineBresenham(254, 184, 646, 184);
    drawLineBresenham(646, 184, 646, 416);
    drawLineBresenham(646, 416, 254, 416);
    drawLineBresenham(254, 416, 254, 184);

    // Corner circles
    glColor3f(1.0, 0.3, 0.3);
    drawFilledCircle(250, 420, 6);
    drawFilledCircle(650, 420, 6);
    drawFilledCircle(250, 180, 6);
    drawFilledCircle(650, 180, 6);

    // Title
    glColor3f(1.0, 0.15, 0.15);
    drawString(393, 360, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

    // Divider line
    glColor3f(0.5, 0.1, 0.1);
    drawLineBresenham(290, 350, 610, 350);

    // Final score
    glColor3f(1.0, 1.0, 1.0);
    drawString(410, 320, scoreStr, GLUT_BITMAP_HELVETICA_18);

    // Divider line
    glColor3f(0.5, 0.1, 0.1);
    drawLineBresenham(290, 250, 610, 250);

    // Instructions
    glColor3f(0.7, 0.7, 0.7);
    drawString(410, 225, "Press R to Restart", GLUT_BITMAP_HELVETICA_12);
    drawString(410, 205, "Press ESC to Quit", GLUT_BITMAP_HELVETICA_12);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Background (always drawn)
    sky();
    if(isNight){
        drawStars();
        drawMoon();
    } else {
        drawSun();
    }
    drawClouds();
    mountains();

    drawBigBushes(40, 200);
    drawBigBushes(870, 200);

    grass();

    drawBushes(20, 110);
    drawBushes(450, 110);
    drawBushes(250, 110);
    drawBushes(850, 110);
    drawBushes(650, 110);
    drawBuildings();

    drawLamp(50, 110);
    drawLamp(400, 110);
    drawLamp(800, 110);

    drawBench(100, 110);
    drawSmallTree(250, 200);
    drawBench(200, 200);

    drawLeftTree(150, 200);
    drawRightTree(750, 200);
    drawSmallTree(700, 180);

    road();
    roadMarkers();
    roadBorder();

    // Game entities
    if (!isGameOver) {
        drawTank(tankX, 25);

        if (muzzleFlash > 0) {
            glColor3f(1.0, 1.0, 0.5);
            drawFilledCircle(tankX + 60, 155, 8);
        }

        drawAllBullets();
        drawAllMissiles();
        drawAllExplosion();
        drawHUD();
    } else {
        // Show tank and remaining explosions
        drawTank(tankX, 25);
        drawAllExplosion();
        drawHUD();
        drawGameOver();
    }

    glFlush();
}

void timer(int value) {
    if(!isPaused && !isGameOver){
        updateBullets();
        bulletSpawnTimer++;
        if(bulletSpawnTimer >= 15){
            spawnBullet();
            bulletSpawnTimer = 0;
        }
        updateMissiles();
        missileSpawnTimer++;
        if(missileSpawnTimer >= 70){
            spawnMissile();
            missileSpawnTimer = 0;
        }

        checkColission();
        if(muzzleFlash > 0){
            muzzleFlash--;
        }
    }

    updateClouds();
    updateExplosions();

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // White background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 900, 0, 600); // Sets the coordinate system to match window size
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 'p': case 'P':
            if (!isGameOver) isPaused ^= 1;
            break;
        case 'n': case 'N':
            isNight ^= 1;
            initStars();
            break;
        case 'a': case 'A':
            if(!isPaused && !isGameOver){
                tankX -= 10.0;
                if(tankX <= -40.0) tankX = -40.0;
            }
            break;
        case 'd': case 'D':
            if(!isPaused && !isGameOver){
                tankX += 10.0;
                if(tankX >= 820.0) tankX = 820.0;
            }
            break;
        case ' ':
            if(!isPaused && !isGameOver){
                spawnBullet();
                bulletSpawnTimer = 0;
            }
            break;
        // RESTART
        case 'r': case 'R':
            tankX = 400.0;
            score = 0;
            lives = 5;
            isPaused = 0;
            isGameOver = 0;
            muzzleFlash = 0;
            bulletSpawnTimer = 0;
            missileSpawnTimer = 0;
            initBullets();
            initMissiles();
            initExplosions();
            break;
        // QUIT
        case 27:   // ESC key
            exit(0);
            break;
        default:
            break;
    }
}

void specialKeys(int key, int x, int y) {
    if (isPaused || isGameOver) return;
    switch (key) {
        case GLUT_KEY_LEFT:
            tankX -= 10.0;
            if(tankX <= -40.0){
                tankX = -40.0;
            }
            break;
        case GLUT_KEY_RIGHT:
            tankX += 10.0;
            if(tankX >= 820.0){
                tankX = 820.0;
            }
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Missile Defence Game");

    init();
    initClouds();
    initBullets();
    initMissiles();
    initExplosions();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
