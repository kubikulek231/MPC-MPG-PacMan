#include "Point3D.h"
#include <cmath>

Point3D::Point3D(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Point3D::Point3D(const Point3D& other) {
	x = other.x;
	y = other.y;
	z = other.z;
}

void Point3D::move(float dx, float dy, float dz) {
	x = x + dx;
	y = y + dy;
	z = z + dz;
}

void Point3D::move(Point3D dPoint) {
	x = x + dPoint.x;
	y = y + dPoint.y;
	z = z + dPoint.z;
}

float Point3D::distanceTo(const Point3D& other) const {
	float dx = x - other.x;
	float dy = y - other.y;
	float dz = z - other.z;
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float Point3D::distanceTo2D(const Point3D& other) const {
	float dx = x - other.x;
	float dz = z - other.z;
	return std::sqrt(dx * dx + dz * dz);
}

float Point3D::getAxisValue(char axis) const {
	if (axis == 'x' || axis == 'X') {
		return this->x;  // Return x value
	}
	if (axis == 'y' || axis == 'Y') {
		return this->y;  // Return y value
	}
	if (axis == 'z' || axis == 'Z') {
		return this->z;  // Return z value
	}
	return std::nanf("");
}

void Point3D::setAxisValue(char axis, float value) {
	if (axis == 'x' || axis == 'X') {
		this->x = value;
		return;
	}
	if (axis == 'y' || axis == 'Y') {
		this->y = value;
		return;
	}
	if (axis == 'z' || axis == 'Z') {
		this->z = value;
		return;
	}
}