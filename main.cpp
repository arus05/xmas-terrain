#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <time.h>

#include "stb_image.h"
#include "glm/glm.hpp"

#include "utils.h"
#include "classes.h"

/* Test variables */
float test = -2;

/* Camera Variables */
float fov = 60;
glm::vec3 camPos(0, -3, 40);
glm::vec3 camFront(0, 0, -1);
glm::vec3 camUp(0, 1, 0);
float yaw = -90;
float pitch = 0;

/* Mouse control variables */
float firstClick = true;
float lastMouseX = 0;
float lastMouseY = 0;
float lastX = 0;
float lastY = 0;

/* Keyboard control variables */
bool moveSun = false;
bool lightsOn = true;
bool displayText = false;

/* Light variables */
float light0_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
float light0_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
float light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
float lightAngle = 90;
float lightRadius = 40;
float lightAngleVelocity = 0.5;
float light0_pos[] = {lightRadius*std::cos(glm::radians(lightAngle)), lightRadius*std::sin(glm::radians(lightAngle)), 0, 1};
float default_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
float default_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
float default_specular[] = { 0.0, 0.0, 0.0, 0.0 };
float default_pos[] = { 0, 50, 0 };
float white_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
float white_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
float white_specular[] = { 0.8, 0.8, 0.8, 1.0 };

/* Sun variables */
float sunRadius = 60;
float sunPos[] = {sunRadius*std::cos(glm::radians(lightAngle)), sunRadius*std::sin(glm::radians(lightAngle)), -40, 1};

/* Materials */
Material mountainMaterial({0.1686, 0.3294, 0.2235}, {0.1686, 0.3294, 0.2235}, { 0.1, 0.1, 0.1 }, {0, 0, 0}, 32);
Material mountainTop({0.7, 0.7, 0.7}, {0.7, 0.7, 0.7}, {0.25, 0.25, 0.25}, {0,0,0}, 0.3*128);
Material leafMaterial({0.0, 0.2553, 0.128}, {0.0, 0.2553, 0.128}, {0.2, 0.2, 0.2}, {0,0,0}, 0.3*128);
Material woodMaterial({0.05375, 0.05, 0.06625}, {0.7882, 0.2000, 0.0000}, {0.332741, 0.328634, 0.346435}, {0,0,0}, 0.3*128);
Material grassMaterial({0.3059, 0.4980, 0.1137}, {0.3059, 0.4980, 0.1137}, {0.332741, 0.328634, 0.346435}, {0,0,0}, 0.3*128);
Material snowMaterial({1*0.4, 1*0.4, 1*0.4}, {1*0.6, 1*0.6, 1*0.6}, {1, 1, 1}, {0,0,0}, 0*128);
Material eyeMaterial({0.2, 0.2, 0.2}, {0, 0, 0}, {0.1, 0.1, 0.1}, {0,0,0}, 0.1*128);
Material carrotMaterial({1.0*0.5, 0.6235*0.5, 0.0*0.5},  {1.0, 0.6235, 0.0}, {0.1, 0.1, 0.1}, {0,0,0}, 0.2*128);
Material redCottonMaterial({0.1f, 0.1f, 0.1f}, {0.8f, 0.0f, 0.0f}, {0.1, 0.1, 0.1}, {0, 0, 0}, 32 );
Material farMountainMaterial({0.5490, 0.5490, 0.6157}, {0.5490, 0.5490, 0.6157}, {0.1, 0.1, 0.1}, {0, 0, 0}, 32 );
Material sunMaterial({1.0, 0.9176, 0.0118}, {1.0, 0.9176, 0.0118}, {0.2, 0.2, 0.2}, {0.25, 0.25, 0.25}, 32);

/* Textures */
GLuint grassTexture;
GLuint snowPuddleTexture;
GLuint presentTexture;

/* Models */
Model tree("./assets/big_tree.obj");
Model xmasTree("./assets/xmas_tree.obj");

/* Array of objects */
std::vector<Snow> snowArr;

void generateTexture(GLuint texture, const char* path) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cout << "Failed to load texture: " << path << std::endl;
        exit(-1);
    }
    if (nrChannels < 3) {
        std::cout << "Texture not available in RGB format: " << path << std::endl;
        exit(-1);
    }

    int format = nrChannels == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void loadTexture() {
    glGenTextures(1, &grassTexture);
    generateTexture(grassTexture, "./assets/grass.jpg");
    glGenTextures(1, &snowPuddleTexture);
    generateTexture(snowPuddleTexture, "./assets/snow_puddle.png");
    glGenTextures(1, &presentTexture);
    generateTexture(presentTexture, "./assets/present.jpg");
}

void useTexture(GLuint texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void resetTexture() {
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void useMaterial(Material material) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, material.getAmbient());
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material.getDiffuse());
    glMaterialfv(GL_FRONT, GL_SPECULAR, material.getSpecular());
    glMaterialfv(GL_FRONT, GL_EMISSION, material.getEmissive());
    glMaterialf(GL_FRONT, GL_SHININESS, material.getShininess());
}

void renderText(const char *string){
    glLineWidth(2);
    const char *c;
    if (lightAngle <= 180) {
        glColor3f(0.694, 0.118, 0.192);
    }
    else {
        glColor3f(1, 1, 1);
    }
    for (c=string; *c != '\0'; c++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }
}

void drawCircle(float radius) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; ++i) {
        float theta = i * 3.14159 / 180;
        float x = radius * std::cos(theta);
        float y = radius * std::sin(theta);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawCube() {
    const GLfloat vertices[] = {
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f, /* BACK FACE */
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, /* FRONT FACE */
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,/* LEFT FACE */
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,

         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,/* RIGHT FACE */
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,/* BOTTOM FACE */
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, /* TOP FACE */
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    };
    glBegin(GL_TRIANGLES);
    for(int i=0; i<sizeof(vertices)/sizeof(vertices[0]); i+=8) {
        glNormal3f(vertices[i+3], vertices[i+4], vertices[i+5]);
        glTexCoord2f(vertices[i+6], vertices[i+7]);
        glVertex3f(vertices[i], vertices[i+1], vertices[i+2]);
    }
    glEnd();
}

void drawTree() {
    glShadeModel(GL_SMOOTH);
    glDisable(GL_COLOR_MATERIAL);
    useMaterial(leafMaterial);

    GLUquadricObj* coneQuadric = gluNewQuadric();
    gluQuadricDrawStyle(coneQuadric, GLU_FILL);
    gluQuadricNormals(coneQuadric, GLU_SMOOTH);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
        gluCylinder(coneQuadric, 1.2, 0, 2.5, 30, 30);
        glTranslatef(0, 0, -1.25);
        gluCylinder(coneQuadric, 1.2*1.15, 0, 3*1.15, 30, 30);
        glTranslatef(0, 0, -1.5);
        gluCylinder(coneQuadric, 1.2*1.35, 0, 3*1.5, 30, 30);
        glTranslatef(0, 0, -1);
        useMaterial(woodMaterial);
        glScalef(1, 1, 2);
        drawCube();
    }
    glPopMatrix();
    gluDeleteQuadric(coneQuadric);

    glShadeModel(GL_FLAT);
}

void drawMountain() {
    glShadeModel(GL_SMOOTH);
    glDisable(GL_COLOR_MATERIAL);
    useMaterial(mountainMaterial);

    GLUquadricObj* coneQuadric = gluNewQuadric();
    gluQuadricDrawStyle(coneQuadric, GLU_FILL);
    gluQuadricNormals(coneQuadric, GLU_SMOOTH);
    gluQuadricTexture(coneQuadric, GLU_TRUE);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
        gluCylinder(coneQuadric, 3.5, 0, 5.0, 8, 4);
    }
    glPopMatrix();
    glPushMatrix();
    {
        gluCylinder(coneQuadric, 3.5, 0, 5.0, 8, 8);
    }
    glPopMatrix();
    glPushMatrix();
    {
        useMaterial(mountainTop);
        glTranslatef(0, 0, 2.55);
        gluCylinder(coneQuadric, 3.5*0.5, 0, 5.0*0.5, 30, 30);
    }
    glPopMatrix();

    gluDeleteQuadric(coneQuadric);
}

void drawFarMountain() {
    glShadeModel(GL_SMOOTH);
    glDisable(GL_COLOR_MATERIAL);;
    useMaterial(farMountainMaterial);

    GLUquadricObj* coneQuadric = gluNewQuadric();
    gluQuadricDrawStyle(coneQuadric, GLU_FILL);
    gluQuadricNormals(coneQuadric, GLU_SMOOTH);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
        gluCylinder(coneQuadric, 3.5, 0, 5.0, 30, 30);
    }
    glPopMatrix();

    gluDeleteQuadric(coneQuadric);
}

void drawCloud() {
    glShadeModel(GL_SMOOTH);
    glDisable(GL_COLOR_MATERIAL);
    useMaterial(snowMaterial);

    GLUquadricObj* sphereQuadric = gluNewQuadric();
    gluQuadricDrawStyle(sphereQuadric, GLU_FILL);
    gluQuadricNormals(sphereQuadric, GLU_SMOOTH);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
        glTranslatef(-2, -1, -0.5);
        gluSphere(sphereQuadric, 2, 30, 30);
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(0, -1, 0);
        gluSphere(sphereQuadric, 1.75, 30, 30);
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(2, -1, -0.5);
        gluSphere(sphereQuadric, 1.8, 30, 30);
    }
    glPopMatrix();
        glPushMatrix();
    {
        glTranslatef(0, 1, -1);
        gluSphere(sphereQuadric, 2.25, 30, 30);
    }
    glPopMatrix();

    gluDeleteQuadric(sphereQuadric);
}

void drawQuad() {
    glBegin(GL_QUADS);
        glTexCoord2f(1, 1);
        glVertex3f(0.5, 0.5, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-0.5, 0.5, 0);
        glTexCoord2f(0, 0);
        glVertex3f(-0.5, -0.5, 0);
        glTexCoord2f(1, 0);
        glVertex3f(0.5, -0.5, 0);
    glEnd();
}

void drawSnowman() {
    resetTexture();
    glShadeModel(GL_SMOOTH);
    glDisable(GL_COLOR_MATERIAL);

    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricTexture(quadric, GLU_TRUE);

    glMatrixMode(GL_MODELVIEW);

    /* Body */
    useMaterial(snowMaterial);
    glPushMatrix();
    {
        glTranslatef(0, 1, 0);
        gluSphere(quadric, 1.5, 30, 30);
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(0, -1, 0);
        gluSphere(quadric, 2, 30, 30);
    }
    glPopMatrix();

    /* Eyes */
    useMaterial(eyeMaterial);
    glPushMatrix();
    {
        glTranslatef(-0.6, 1.6, 1.25);
        gluSphere(quadric, 0.2, 10, 10);
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(0.6, 1.6, 1.25);
        gluSphere(quadric, 0.2, 10, 10);
    }
    glPopMatrix();

    /* Buttons */
    glPushMatrix();
    {
        glTranslatef(0, -0.4, 1.9);
        gluSphere(quadric, 0.2, 10, 10);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(0, -1, 2);
        gluSphere(quadric, 0.2, 10, 10);
    }
    glPopMatrix();

    /* Nose */
    useMaterial(carrotMaterial);
    glPushMatrix();
    {
        glTranslatef(0, 1.1, 1.2);
        gluCylinder(quadric, 0.2, 0, 1.25, 8, 8);
    }
    glPopMatrix();

    /* Hat */
    useMaterial(redCottonMaterial);
    glPushMatrix();
    {
        glTranslatef(0, 2.25, 0);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(quadric, 0.75, 0, 1.5, 8, 8);
    }
    glPopMatrix();

    useMaterial(snowMaterial);
    glPushMatrix();
    {
        glTranslatef(0, 3.7, 0);
        gluSphere(quadric, 0.3, 10, 10);
    }
    glPopMatrix();

    /* Snow puddle */
    useMaterial(snowMaterial);
    useTexture(snowPuddleTexture);
    glEnable(GL_BLEND);
    glPushMatrix();
    {
        glTranslatef(0, -1.95, 0);
        glRotatef(90, 1, 0, 0);
        glScalef(12, 12, 1);
        drawQuad();
    }
    resetTexture();
    glDisable(GL_BLEND);
    glPopMatrix();

    gluDeleteQuadric(quadric);
}

void drawSun() {
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    useMaterial(sunMaterial);
    gluSphere(quadric, 15, 15, 15);

}

void drawSnow() {
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    useMaterial(snowMaterial);

    for (Snow snow : snowArr) {
        glPushMatrix();
        {
            glTranslatef(snow.getX(), snow.getY(), snow.getZ());
            gluSphere(quadric, snow.getRadius(), 8, 8);
        }
        glPopMatrix();
     }
}

void display(void) {
    if (lightAngle <= 180) {
        glClearColor(0.6667, 0.8000, 1.0000, 1);
    }
    else {
        glClearColor(0.04, 0.08, 0.18, 1);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double w = glutGet( GLUT_WINDOW_WIDTH );
    double h = glutGet( GLUT_WINDOW_HEIGHT );
    double ar = w/h;

/* ----------------------------- 2D rendering ----------------------------- */
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDepthMask( GL_FALSE );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, 500, 0, 500/ar, -1, 1 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

/* ----------------------------- 3D rendering ----------------------------- */

    /* Setup for 3D rendering */
    glDepthMask( GL_TRUE );
    glEnable( GL_DEPTH_TEST );

    /* Setup lighting */
    glLightfv(GL_LIGHT1, GL_AMBIENT, default_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, default_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, default_specular);
    glLightfv(GL_LIGHT2, GL_AMBIENT, white_ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, white_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, white_specular);
    glLightfv(GL_LIGHT3, GL_AMBIENT, white_ambient);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, white_diffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, white_specular);

    glEnable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);

    if (lightAngle <= 180) {
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    }
    else {
        glDisable(GL_LIGHT0);
    }

    /* Projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, ar, 0.1, 100);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    /* Rendering */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glm::vec3 camTarget = camFront + camPos;
    gluLookAt(camPos.x, camPos.y, camPos.z, camTarget.x, camTarget.y, camTarget.z, camUp.x, camUp.y, camUp.z);

    /* Draw stuff that does not require lighting */
    glDisable(GL_LIGHTING);

    /* ----------------------- Text ----------------------- */
    if (displayText) {
        glPushMatrix();
        {
            glTranslatef(-40, 30, -35);
            glScalef(0.08, 0.08, 0.08);
            renderText("Merry Christmas");
        }
        glPopMatrix();
    }

    /* Draw stuff that does require lighting */
    glEnable(GL_LIGHTING);

    /* ----------------------- Place Lights ----------------------- */
    glPushMatrix();
    {
        glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
        useMaterial(leafMaterial);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glLightfv(GL_LIGHT1, GL_POSITION, default_pos);
        useMaterial(leafMaterial);
    }
    glPopMatrix();

    /* ----------------------- Floor ----------------------- */
    useTexture(grassTexture);
    useMaterial(snowMaterial);
    glDisable(GL_COLOR_MATERIAL);
    glPushMatrix();
    {
        glBegin(GL_QUADS);
        glTexCoord2f(1,1);
        glVertex3f(50, -10, -50);
        glTexCoord2f(0,1);
        glVertex3f(-50, -10, -50);
        glTexCoord2f(0,0);
        glVertex3f(-50, -10, 50);
        glTexCoord2f(1,0);
        glVertex3f(50, -10, 50);
        glEnd();
    }
    glPopMatrix();
    resetTexture();

    /* ----------------------- Huge Trees ----------------------- */
    glPushMatrix();
    {
        glTranslatef(25, -9.8, -10);
        glScalef(0.5, 0.5, 0.5);
        glRotatef(-55, 0, 1, 0);
        tree.drawModel();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(25, -9.8, 25);
        glScalef(0.35, 0.35, 0.35);
        tree.drawModel();
    }
    glPopMatrix();

    /* ----------------------- Sun ----------------------- */
    glPushMatrix();
    {
        glTranslatef(sunPos[0], sunPos[1], sunPos[2]);
        drawSun();
    }
    glPopMatrix();

    /* ----------------------- Mountains ----------------------- */
    glPushMatrix();
    {
        glTranslatef(10, -10, -25);
        glScalef(3, 3, 3);
        glRotatef(-90, 1, 0, 0);
        drawMountain();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-25, -10, -30);
        glScalef(1.5, 1.5, 1.5);
        glRotatef(-90, 1, 0, 0);
        drawMountain();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-20, -10, -15);
        glScalef(2.5, 2.5, 2.5);
        glRotatef(-90, 1, 0, 0);
        drawMountain();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-10, -10, -35);
        glScalef(3.5, 4.5, 3.5);
        glRotatef(-90, 1, 0, 0);
        drawMountain();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-21, -10, 35);
        glScalef(3.25, 3.25, 3.25);
        glRotatef(-90, 1, 0, 0);
        drawMountain();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(32, -10, 0);
        glScalef(2.5, 2.5, 2.5);
        glRotatef(-90, 1, 0, 0);
        drawMountain();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-41, -10, 10);
        glScalef(2.93, 4.23, 2.85);
        glRotatef(-90, 1, 0, 0);
        drawMountain();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-40, -15, -10);
        glScalef(3.93, 2.23, 3.85);
        glRotatef(-90, 1, 0, 0);
        drawMountain();
    }
    glPopMatrix();

    resetTexture();

    /* ----------------------- BG Mountains ----------------------- */

    glPushMatrix();
    {
        glTranslatef(-23, -10, -45);
        glScalef(2.93, 4.23, 2.85);
        glRotatef(-90, 1, 0, 0);
        drawFarMountain();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(19, -10, -45);
        glScalef(4.93, 3.23, 4.93);
        glRotatef(-90, 1, 0, 0);
        drawFarMountain();
    }
    glPopMatrix();


    /* ----------------------- Trees ----------------------- */

    glPushMatrix();
    {
        glTranslatef(0, -6, 0);
        glRotatef(-90, 1, 0, 0);
        drawTree();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(10, -6, 0);
        glRotatef(-90, 1, 0, 0);
        drawTree();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(10, -6, -10);
        glRotatef(-90, 1, 0, 0);
        drawTree();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(15, -6, 10);
        glRotatef(-90, 1, 0, 0);
        drawTree();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-22, -6, 15);
        glRotatef(-90, 1, 0, 0);
        drawTree();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-28, -6, -5);
        glRotatef(-90, 1, 0, 0);
        drawTree();
    }
    glPopMatrix();

    /* ----------------------- Clouds ----------------------- */

    glPushMatrix();
    {
        glTranslatef(0, 10, -20);
        glScalef(1.25, 1, 1);
        drawCloud();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-25, 13.5, 0);
        glScalef(1.25, 1, 1.125);
        glScalef(1.15, 1.15, 1.15);
        glRotatef(25, 0, 1, 0);
        drawCloud();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-25, 12.5, 21);
        glScalef(1.35, 1, 1.175);
        glScalef(1.23, 1.23, 1.23);
        glRotatef(90, 0, 1, 0);
        drawCloud();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(37.5, 14.5, 10);
        glScalef(1.35, 1, 1.35);
        glScalef(1.3, 1.3, 1.3);
        drawCloud();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(29.5, 27.5, -45);
        glScalef(3.5, 1.75, 3.5);
        glScalef(1.3, 1.3, 1.3);
        drawCloud();
    }
    glPopMatrix();

    /* ----------------------- Snow man ----------------------- */
    glPushMatrix();
    {
        glTranslatef(-6, -2, 4);
        glScalef(0.5, 0.5, 0.5);
        drawCloud();
    }
    glPopMatrix();
    glPushMatrix();
    {
        glTranslatef(-6, -8, 4);
        drawSnowman();
    }
    glPopMatrix();

     /* ----------------------- Snow ----------------------- */
    drawSnow();

    /* ----------------------- Xmas Tree ----------------------- */

    /* Setup lighting */
    if (lightsOn) {
        glEnable(GL_LIGHT2);
        glEnable(GL_LIGHT3);
    }
    else {
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);
    }
    float xmasTreeLightPos[] = {-17, 0, 0};
    float xmasTreeLightPos2[] = { 3.6, -2, 10 };
    glLightfv(GL_LIGHT2, GL_POSITION, xmasTreeLightPos);
    glLightfv(GL_LIGHT3, GL_POSITION, xmasTreeLightPos2);

    glPushMatrix();
    {
        glTranslatef(-17, -9.6, 0);
        glScalef(2, 2, 2);
        xmasTree.drawModel();
    }
    glPopMatrix();

    /* Presents */
    useTexture(presentTexture);
    useMaterial(redCottonMaterial);
    glPushMatrix();
    {
        glTranslatef(-20, -9.05, 2.45);
        glScalef(2, 2, 2);
        glRotatef(8, 0, 1, 0);
        drawCube();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-16, -9.3, 4.35);
        glScalef(1.5, 1.5, 1.5);
        glRotatef(-10, 0, 1, 0);
        drawCube();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-20, -7.5, 2.45);
        glScalef(1.25, 1.25, 1.25);
        glRotatef(30, 0, 1, 0);
        drawCube();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-11, -8.45, 0);
        glScalef(3.25, 3.25, 3.25);
        glRotatef(-40, 0, 1, 0);
        drawCube();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-14, -8.95, 4);
        glScalef(2.25, 2.25, 2.25);
        glRotatef(75, 0, 1, 0);
        drawCube();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-17.5, -9.15, 2.9);
        glScalef(1.75, 1.75, 1.75);
        glRotatef(-60, 0, 1, 0);
        drawCube();
    }
    glPopMatrix();
    resetTexture();

    glutSwapBuffers();
}

void key(unsigned char key, int x, int y) {
    float camSpeed = 1.25;
    glm::vec3 rightDir = glm::normalize(glm::cross(camFront, camUp));
    switch(key) {
        case 'w':
        case 'W':
            camPos += camFront*camSpeed;
            break;
        case 's':
        case 'S':
            camPos -= camFront*camSpeed;
            break;
        case 'a':
        case 'A':
            camPos -= rightDir*camSpeed;
            break;
        case 'd':
        case 'D':
            camPos += rightDir*camSpeed;
            break;
        case ' ':
            moveSun = !moveSun;
            break;
        case 'l':
        case 'L':
            lightsOn = !lightsOn;
            break;
        case 't':
        case 'T':
            displayText = !displayText;
            break;
    }
}

void timer(int val) {
    if (moveSun) {
        lightAngle += lightAngleVelocity;
        if (lightAngle > 360) {
            lightAngle = 0;
        }
        light0_pos[0] = lightRadius*std::cos(glm::radians(lightAngle));
        light0_pos[1] = lightRadius*std::sin(glm::radians(lightAngle));
        sunPos[0] = sunRadius*std::cos(glm::radians(lightAngle));
        sunPos[1] = sunRadius*std::sin(glm::radians(lightAngle));
    }

    for (int i=0; i<snowArr.size(); i++) {
        snowArr[i].updatePos();
    }

    glutPostRedisplay();
    glutTimerFunc(1000/30, timer, 0);
}

void special(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        test += 0.05;
    }
    else if (key == GLUT_KEY_DOWN) {
        test -= 0.05;
    }
    std::cout << test << " " << std::endl;
}

void mousewheel(int button, int dir, int x, int y) {
    if (dir > 0)
    {
        fov -= 2;
        if (fov < 10) fov = 10;
    }
    else
    {
        fov += 2;
        if (fov > 80) fov = 80;
    }
}

void motion(int x, int y) {
    if (firstClick) {
        lastX = x;
        lastY = y;
        firstClick = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;

    float sensitivity = 0.25f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(direction);
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_UP) {
        firstClick = true;
    }
}

/* Subroutines that are carried out only once */
void myInit() {
    /* General init */
    srand(time(NULL));

    /* openGL init */
    glClearColor(0.6667, 0.8000, 1.0000, 1);
    glEnable(GL_NORMALIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    loadTexture();

    /* Generate snow */
    for (int i=0; i<250; i++) {
        snowArr.push_back(Snow());
    }
}

void printInstructions() {
    const char* instructions = "\n-----------------------------------------------------------------\n\n"
                         "Merry Christmas!\n"
                         "Instructions:\n"
                         "Press W,A,S,D keys to move around the scene\n"
                         "Use your mouse to rotate the camera around by clicking and dragging\n"
                         "Press SPACE to move and stop the sun (for night and day transition)\n"
                         "Press L to turn on and off Christmas tree lights\n"
                         "Press T to show and hide the text\n";

    std::cout << instructions << std::endl;
}

/* Program entry point */
int main(int argc, char *argv[])
{
    printInstructions();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutCreateWindow("Christmas Poster");

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize glew" << std::endl;
        exit(-1);
    }

    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutTimerFunc(0, timer, 0);
    glutMouseWheelFunc(mousewheel);
    glutMotionFunc(motion);
    glutMouseFunc(mouse);

    myInit();

    glutMainLoop();

    return EXIT_SUCCESS;
}
