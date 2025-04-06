#include "BoundingBox3D.h"

BoundingBox3D::BoundingBox3D(Point3D min, Point3D max) {
	this->min = min;
	this->max = max;
}

BoundingBox3D::BoundingBox3D(const BoundingBox3D& other) {
	this->min = other.min;
	this->max = other.max;
}

void BoundingBox3D::move(float dx, float dy, float dz) {
	min.move(dx, dy, dz);
	max.move(dx, dy, dz);
}

void BoundingBox3D::move(Point3D dPoint) {
	min.move(dPoint);
	max.move(dPoint);
}

// Check if two BoundingBox3D objects intersect
bool BoundingBox3D::intersects(const BoundingBox3D& other) const {
    // Check if there's no overlap on any of the axes
    if (max.x < other.min.x || min.x > other.max.x) return false;
    if (max.y < other.min.y || min.y > other.max.y) return false;
    if (max.z < other.min.z || min.z > other.max.z) return false;

    // If no condition is met for non-intersection, they intersect
    return true;
}

BoundingBox3D BoundingBox3D::getAbsoluteBoundingBox(Point3D bottomLeftOrigin) const {
    Point3D bbMin = bottomLeftOrigin;  // Origin is the lower-left corner
    Point3D bbMax = Point3D(
        bottomLeftOrigin.x + this->max.x,  // Max x = origin.x + width
        bottomLeftOrigin.y + this->max.y,  // Max y = origin.y + height
        bottomLeftOrigin.z + this->max.z   // Max z = origin.z + depth
    );

    return BoundingBox3D(bbMin, bbMax);
}