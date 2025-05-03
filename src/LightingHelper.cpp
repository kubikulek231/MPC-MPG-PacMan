#include "LightingHelper.h"

void LightingHelper::setMaterial(
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

void LightingHelper::resetMaterial(GLenum face) {
    static const GLfloat zeroColor[] = { 0.f, 0.f, 0.f, 1.f };

    glMaterialfv(face, GL_AMBIENT, zeroColor);
    glMaterialfv(face, GL_DIFFUSE, zeroColor);
    glMaterialfv(face, GL_SPECULAR, zeroColor);
    glMaterialfv(face, GL_EMISSION, zeroColor);
    glMaterialf(face, GL_SHININESS, 0.0f);
}