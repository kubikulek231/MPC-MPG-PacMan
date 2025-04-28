#include "MenuItem.h"
#include "glft2/TextRenderer.hpp"

MenuItem::MenuItem(std::shared_ptr<glft2::font_data> font, const std::string& txt, float x, float y, float width, float height, float textScale, float textX, float textY) {
    this->text = txt;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->textX = textX;
    this->textY = textY;
    this->textScale = textScale;
    this->font = font;
}

// Method to check if a given point (px, py) intersects with the bounding box
void MenuItem::update(int mouseX, int mouseY, bool entered) {
    if (textNeedsCentering) { calculateCenteredTextDimensions(); textNeedsCentering = false; }
    select(mouseX, mouseY, entered);
}

void MenuItem::select(float mouseX, float mouseY, bool entered) {
    bool intersects = (mouseX >= x && mouseX <= x + width &&
        mouseY >= y && mouseY <= y + height);

    if (!intersects) {
        this->selected = false;
        this->entered = false;
        return;
    }

    if (selectable) {
        this->selected = true;
        this->entered = entered;
        return;
    }

    this->selected = false;
    this->entered = false;
    return;
}


void MenuItem::calculateCenteredTextDimensions() {
    float textW, textH;
    glft2::measureText(*this->font, text, &textW, &textH, textScale);
    textWidth = textW;
    textHeight = textH;

    if (!centerText) { return; }
    // Does text fit the box?
    if (width < textWidth) { return; }
    if (height < textHeight) { return; }
    this->textX = x + (width - textWidth) / 2.0f;
    this->textY = y + (height - textHeight) / 2.0f;
}

void MenuItem::updateXY(float x, float y) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    calculateCenteredTextDimensions();
}

void MenuItem::render() {
    glPushMatrix(); // Good practice to isolate matrix changes
    glPixelZoom(-1.0f, 1.0f);

    // --- Enable transparency blending ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- Draw background rectangle with transparency ---
    glColor4ub(backgroundColorRGBA[0], backgroundColorRGBA[1], backgroundColorRGBA[2], backgroundColorRGBA[3]);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // --- Draw text (no transparency for text, just RGB colors) ---
    if (selected) {
        glColor3ub(selectedTextColorRGB[0], selectedTextColorRGB[1], selectedTextColorRGB[2]);
    }
    else {
        glColor3ub(textColorRGB[0], textColorRGB[1], textColorRGB[2]);
    }

    glft2::render2D(*font, textX, textY, text, textScale);

    // --- Disable blending after rendering ---
    glDisable(GL_BLEND);

    glPopMatrix();
}
