#ifndef GameLighting_H
#define GameLighting_H

#include "gl_includes.h"

static class GameLighting {
private:
    static void initSceneLight();
    // lightPos(x, y, z, w) | lightDir(x, y, z)
    static void initCameraLight();
public:
    static void init();
    static void updateCameraLight(GLfloat* lightPos, GLfloat* lightDir);
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
#endif