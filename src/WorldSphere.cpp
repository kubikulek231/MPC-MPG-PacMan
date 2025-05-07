#include "WorldSphere.h"
#include <iostream>
#include "GameLighting.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Destructor to clean up resources
WorldSphere::~WorldSphere() {
    glDeleteTextures(1, &textureID);
    gluDeleteQuadric(quadric);
}

// Method to init WorldSphere instance, loads texture
void WorldSphere::init() {
    int width, height, channels;
    unsigned char* data = stbi_load(TEXTURE_PATH, &width, &height, &channels, 0);
    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    if (data) {
        glGenTextures(1, &textureID);            // Generate a texture ID
        glBindTexture(GL_TEXTURE_2D, textureID); // Bind the texture to OpenGL

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload the texture to OpenGL
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // Free the image data as we no longer need it
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << TEXTURE_PATH << std::endl;
    }
}

// Method to render the sphere with the loaded texture applied
void WorldSphere::render() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GameLighting::setMaterial(GL_FRONT_AND_BACK, LIGHT_AMBIENT, LIGHT_DIFFUSE, LIGHT_SPECULAR, LIGHT_EMISSION, LIGHT_SHININESS);
    glPushMatrix();
        // Optional: scale if your world requires it
        glTranslatef(-1.0f, 0.0f, 0.0f);
        // Render the sphere from inside
        gluSphere(quadric, SPHERE_RADIUS, 32, 32);

    glPopMatrix();
    GameLighting::resetMaterial(GL_FRONT_AND_BACK);

    glDisable(GL_TEXTURE_2D);
}