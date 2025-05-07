#ifndef WORLDSPHERE_H
#define WORLDSPHERE_H

#include "gl_includes.h"

class WorldSphere {
public:
    static constexpr const char* TEXTURE_PATH = "assets/textures/stars.png";
    static constexpr float SPHERE_RADIUS = 100.0f;
    // Set the lighting to neutral white with no shininess
    static constexpr GLfloat LIGHT_AMBIENT[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    static constexpr GLfloat LIGHT_DIFFUSE[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    static constexpr GLfloat LIGHT_SPECULAR[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    static constexpr GLfloat LIGHT_EMISSION[4] = { 0.1f, 0.1f, 0.1f, 0.0f };
    static constexpr float LIGHT_SHININESS = 0.0f; // Zero shininess (matte finish)


    static WorldSphere& getInstance() {
        static WorldSphere instance;
        return instance;
    }

    // Destructor to clean up resources
    ~WorldSphere();

    void init();
    void render();

private:
    WorldSphere() = default;
    WorldSphere(const WorldSphere&) = delete;

    GLUquadric* quadric;

    GLuint textureID;  // OpenGL texture ID
};

#endif // WORLDSPHERE_H
