#include "Entity.h"
#include <iostream>

Entity::Entity(Point3D origin, BoundingBox3D boundingBox) {
	this->origin = origin;
	this->boundingBox = boundingBox;
}

Entity::Entity(const Entity& other) {
    origin = other.origin;
    boundingBox = other.boundingBox;
}

Point3D Entity::getOrigin() const {
    return origin;
}

BoundingBox3D Entity::getBoundingBox() const {
    return boundingBox;
}

BoundingBox3D Entity::getAbsoluteBoundingBox() const {
    return this->boundingBox.getAbsoluteBoundingBox(origin);
}

void Entity::setBoundingBox(Point3D newMin, Point3D newMax) {
	boundingBox.min = newMin;
	boundingBox.max = newMax;
}

void Entity::setOrigin(Point3D newOrigin) {
	origin.x = newOrigin.x;
	origin.y = newOrigin.y;
	origin.z = newOrigin.z;
}

Point3D Entity::getMovedOrigin(Point3D dPoint) const {
	// Create copy using copy constructor
	Point3D newPoint = Point3D(this->origin);
	newPoint.move(dPoint);
    return newPoint;
}

BoundingBox3D Entity::getMovedBoundingBox(Point3D dPoint) const {
	// Create copy using copy constructor
	BoundingBox3D newBoundingBox = BoundingBox3D(this->boundingBox);
	newBoundingBox.move(dPoint);
    return newBoundingBox;
}

BoundingBox3D Entity::getAbsoluteMovedBoundingBox(Point3D dPoint) const {
    // Create copy using copy constructor
    BoundingBox3D newBoundingBox = BoundingBox3D(this->boundingBox);
    newBoundingBox.move(dPoint);
    return newBoundingBox.getAbsoluteBoundingBox(newBoundingBox.min);

}

Point3D Entity::getAbsoluteCenterPoint() const {
    BoundingBox3D absoluteBoundingBox = boundingBox.getAbsoluteBoundingBox(origin);
    return (absoluteBoundingBox.min + absoluteBoundingBox.max) / 2.0f;
}

// Check if this entity intersects with another entity
bool Entity::intersects(const Entity& otherEntity) const {
    // Adjust bounding boxes relative to the origin (lower-left corner)
    BoundingBox3D thisBoundingBox = this->getAbsoluteBoundingBox();
    BoundingBox3D otherBoundingBox = otherEntity.getAbsoluteBoundingBox();

    return thisBoundingBox.intersects(otherBoundingBox);
}

void Entity::renderBoundingBox() const {
    BoundingBox3D abb = getAbsoluteBoundingBox();
    glPushMatrix();

    // Enable transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the color with 10% opacity (red in this case)
    glColor4f(1.0f, 0.0f, 0.0f, 0.1f);

    glBegin(GL_QUADS);

    // Front face (z = max.z)
    glVertex3f(abb.min.x, abb.min.y, abb.max.z);
    glVertex3f(abb.max.x, abb.min.y, abb.max.z);
    glVertex3f(abb.max.x, abb.max.y, abb.max.z);
    glVertex3f(abb.min.x, abb.max.y, abb.max.z);

    // Back face (z = min.z)
    glVertex3f(abb.min.x, abb.min.y, abb.min.z);
    glVertex3f(abb.max.x, abb.min.y, abb.min.z);
    glVertex3f(abb.max.x, abb.max.y, abb.min.z);
    glVertex3f(abb.min.x, abb.max.y, abb.min.z);

    // Top face (y = max.y)
    glVertex3f(abb.min.x, abb.max.y, abb.min.z);
    glVertex3f(abb.max.x, abb.max.y, abb.min.z);
    glVertex3f(abb.max.x, abb.max.y, abb.max.z);
    glVertex3f(abb.min.x, abb.max.y, abb.max.z);

    // Bottom face (y = min.y)
    glVertex3f(abb.min.x, abb.min.y, abb.min.z);
    glVertex3f(abb.max.x, abb.min.y, abb.min.z);
    glVertex3f(abb.max.x, abb.min.y, abb.max.z);
    glVertex3f(abb.min.x, abb.min.y, abb.max.z);

    // Right face (x = max.x)
    glVertex3f(abb.max.x, abb.min.y, abb.min.z);
    glVertex3f(abb.max.x, abb.max.y, abb.min.z);
    glVertex3f(abb.max.x, abb.max.y, abb.max.z);
    glVertex3f(abb.max.x, abb.min.y, abb.max.z);

    // Left face (x = min.x)
    glVertex3f(abb.min.x, abb.min.y, abb.min.z);
    glVertex3f(abb.min.x, abb.max.y, abb.min.z);
    glVertex3f(abb.min.x, abb.max.y, abb.max.z);
    glVertex3f(abb.min.x, abb.min.y, abb.max.z);

    glEnd();

    // Disable transparency
    glDisable(GL_BLEND);

    glPopMatrix();
}

// Debug function to render the origin as a small sphere
void Entity::renderOrigin(bool renderCoordinates) const {
    glPushMatrix();

    // Translate to the origin position
    glTranslatef(origin.x, origin.y, origin.z);

    // Color the origin as a debug marker (e.g., green)
    glColor3f(0.0f, 1.0f, 0.0f);  // Green for origin

    // Render a small sphere at the origin
    glutSolidSphere(0.1f, 10, 10);  // A small sphere for the origin

    glPopMatrix();

    if (renderCoordinates) {
        // Switch to 2D text rendering
        glDisable(GL_DEPTH_TEST);

        glPushMatrix();
        
        // Offset the text from the origin by adding an offset to the x and y positions
        float textOffsetX = 0.2f;  // X offset
        float textOffsetY = 0.2f;  // Y offset
        float textOffsetZ = 1.0f;
        glTranslatef(origin.x + textOffsetX, origin.y + textOffsetY, origin.z + textOffsetZ); // Offset the text position

        // Render the coordinates text
        renderText("Origin: (" + std::to_string(origin.x) + ", " + std::to_string(origin.y) + ", " + std::to_string(origin.z) + ")");
        
        glPopMatrix();

        glEnable(GL_DEPTH_TEST);

        std::cout << "Origin: ("
            << origin.x << ", "
            << origin.y << ", "
            << origin.z << ")"
            << std::endl;
    }
}

void Entity::renderText(const std::string& text) const {
    // Use GLUT to render text
    for (size_t i = 0; i < text.length(); ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}