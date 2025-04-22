#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace glft2 {

    // Stores data for a font rendered with FreeType
    struct font_data {
        float h;                      // Font height
        std::vector<GLuint> textures; // Texture IDs for glyphs
        GLuint list_base;             // Start of display lists
        float char_widths[128] = {};  // Cached character widths

        // Load font from file at specified height
        void init(const char* fname, unsigned int h);

        // Free GPU resources
        void clean();
    };

    // Render 2D text at (x, y) with optional scale
    void render2D(const font_data& ft_font, float x, float y, const std::string& text, float scale = 1.0f);

    // Render 3D text at origin with optional scale (uses modelview)
    void render3D(const font_data& ft_font, const std::string& text, float scale = 1.0f);

    // Measure width and height of given text
    void measureText(const font_data& ft_font, const std::string& text, float* out_width, float* out_height, float scale = 1.0f);
}