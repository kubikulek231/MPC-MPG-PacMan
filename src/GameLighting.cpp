#include "GameLighting.h"
#include <iostream>

void GameLighting::setMaterial(
    GLenum face,
    const GLfloat* ambient,
    const GLfloat* diffuse,
    const GLfloat* specular,
    const GLfloat* emission,
    GLfloat shininess
) {
    if (ambient)   glMaterialfv(face, GL_AMBIENT, ambient);
    if (diffuse)   glMaterialfv(face, GL_DIFFUSE, diffuse);
    if (specular)  glMaterialfv(face, GL_SPECULAR, specular);
    if (emission)  glMaterialfv(face, GL_EMISSION, emission);
    glMaterialf(face, GL_SHININESS, shininess);
}

void GameLighting::resetMaterial(GLenum face) {
    static const GLfloat zeroColor[] = { 0.f, 0.f, 0.f, 1.f };

    glMaterialfv(face, GL_AMBIENT, zeroColor);
    glMaterialfv(face, GL_DIFFUSE, zeroColor);
    glMaterialfv(face, GL_SPECULAR, zeroColor);
    glMaterialfv(face, GL_EMISSION, zeroColor);
    glMaterialf(face, GL_SHININESS, 0.0f);
}

void GameLighting::init() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

    initSceneLight();
    initCameraLight();
}

void GameLighting::initSceneLight() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT2);

    GLfloat lightPos[] = { 15.0f, 20.0f, 0.0f, 1.0f }; 
    GLfloat lightDir[] = { -1.0f, -1.0f, 0.0f };

    // Light color
    GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    // Spotlight settings
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 120.0f);          // wide beam
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 0.0f);        // focus intensity (0-128)

    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.3f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.f);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.f);

    // Apply light settings
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, specular);

    glPushMatrix();
        glLoadIdentity();
        glLightfv(GL_LIGHT2, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lightDir);
    glPopMatrix();
}

void GameLighting::initCameraLight() {
    // Setup camera spotlight as LIGHT1
    glEnable(GL_LIGHT1);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 48.0f); // Narrower cone for spotlight
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 64.0f); // Sharper focus at center

    GLfloat amb[] = { 0.05f, 0.05f, 0.05f, 1.0f };  // Dim ambient to simulate focused light
    GLfloat dif[] = { 0.1f, 0.1f, 0.1f, 1.0f };  // Bright diffuse
    GLfloat spc[] = { 0.2f, 0.2f, 0.2f, 1.0f };  // Sharp specular
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spc);

    // Use more realistic attenuation to limit spotlight reach
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.80f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.f);
}


void GameLighting::updateCameraLight(GLfloat lightPos[4], GLfloat lightDir[3]) {
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDir);

    // Debug: draw a glowing sphere at the light source
    //glPushMatrix();
    //glTranslatef(camPos[0], camPos[1], camPos[2]);
    //GLfloat z[] = { 0,0,0,1 };
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, z);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, z);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, z);
    //GLfloat em[] = { 1.0f,1.0f,0.2f,1.0f };
    //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, em);
    //glutSolidSphere(0.3f, 16, 16);
    //GLfloat em0[] = { 0,0,0,1 };
    //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, em0);
    //glPopMatrix();
}
