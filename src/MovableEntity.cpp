#include "MovableEntity.h"

MovableEntity::MovableEntity(Point3D origin, BoundingBox3D boundingBox) : Entity(origin, boundingBox) {
}

MovableEntity::MovableEntity(const MovableEntity& other) {
}

void MovableEntity::move(float dx, float dy, float dz) {
	origin.move(dx, dy, dz);
}

void MovableEntity::move(Point3D dPoint) {
	origin.move(dPoint);
}

void MovableEntity::moveX(float dx) {
    origin.move(dx, 0, 0);
}

void MovableEntity::moveY(float dY) {
    origin.move(0, dY, 0);
}

void MovableEntity::moveZ(float dZ) {
    origin.move(0, 0, dZ);
}