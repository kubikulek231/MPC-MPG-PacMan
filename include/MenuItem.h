#ifndef MENUITEM_H
#define MENUITEM_H

#include "gl_includes.h"
#include <glft2/TextRenderer.hpp>
#include <vector>
#include <string>
#include <memory>

class MenuItem {
private:
    std::shared_ptr<glft2::font_data> font;

    bool selected = false;
    bool entered = false;

    std::string text;
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    float textX = 0.0f;
    float textY = 0.0f;
    float textScale = 0.5f;
    bool centerText = true;
    bool textNeedsCentering = true;

    float textWidth = 0.0f;
    float textHeight = 0.0f;

    GLubyte textColorRGB[3] = { 255, 255, 255 };
    GLubyte selectedTextColorRGB[3] = { 180, 240, 255 };
    GLubyte backgroundColorRGBA[4] = { 255, 0, 0, 255 };

    bool selectable = true;

    void select(float mouseX, float mouseY, bool entered);

public:
    MenuItem(std::shared_ptr<glft2::font_data> font, const std::string& txt, float x, float y, float width, float height, float textScale, float textX = 0.0f, float textY = 0.0f);
    void update(int mouseX, int mouseY, bool entered);
    void render();
    void calculateCenteredTextDimensions();
    void updateXY(float x, float y);

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    bool isSelectable() const { return selectable; }
    const std::string& getText() const { return text; }
    bool isSelected() const { return selected; }
    bool isEntered() const { return entered; }

    // Setters
    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }
    void setWidth(float newWidth) { width = newWidth; }
    void setHeight(float newHeight) { height = newHeight; }
    void setSelectable(bool value) { 
        selected = false;
        entered = false;
        selectable = value; 
    }
    void setTextX(float newX) {
        this->textX = newX;
    }

    void setTextY(float newY) {
        this->textY = newY;
    }

    void setTextColor(GLubyte r, GLubyte g, GLubyte b) {
        textColorRGB[0] = r;
        textColorRGB[1] = g;
        textColorRGB[2] = b;
    }

    void setSelectedTextColor(GLubyte r, GLubyte g, GLubyte b) {
        selectedTextColorRGB[0] = r;
        selectedTextColorRGB[1] = g;
        selectedTextColorRGB[2] = b;
    }

    void setBackgroundColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
        backgroundColorRGBA[0] = r;
        backgroundColorRGBA[1] = g;
        backgroundColorRGBA[2] = b;
        backgroundColorRGBA[3] = a;
    }

    void setTextScale(float scale) {
        textScale = scale;
        calculateCenteredTextDimensions();
    }

    void setCenterText(bool centerText) {
        this->centerText = centerText;
    }
};

#endif
