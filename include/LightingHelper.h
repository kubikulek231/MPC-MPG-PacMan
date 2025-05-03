// FadeTimer.h
#ifndef LIGHTINGHELPER_H
#define LIGHTINGHELPER_H

#include "gl_includes.h"

static class LightingHelper {
public:
    static void setMaterial(
        GLenum face,
        const GLfloat* ambient,
        const GLfloat* diffuse,
        const GLfloat* specular,
        const GLfloat* emission,
        GLfloat shininess
    );

    static void resetMaterial(GLenum face);
};
#endif // LIGHTINGHELPER_H