#include "GameLighting.h"

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
    glEnable(GL_LIGHT2);
    GLfloat dirLightPos[] = { 6.0f, 20.0f, 0.0f, 0.0f };
    GLfloat dirLightColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT2, GL_POSITION, dirLightPos);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, dirLightColor);
}

void GameLighting::initCameraLight() {
    // Setup camera spotlight as LIGHT1
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 35.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 300.0f);

    GLfloat amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // White light
    GLfloat spc[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // Strong specular highlights
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spc);

    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.15f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.00015f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.00015f);
}

void GameLighting::updateCameraLight(GLfloat* lightPos, GLfloat* lightDir) {
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
