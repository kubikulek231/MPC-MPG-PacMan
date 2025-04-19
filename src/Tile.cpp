#include "Tile.h"
#include "MapFactory.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>


Tile::Tile(TileType tileType, Point3D tileOrigin, BoundingBox3D tileBoundingBox, int tileRow, int tileCol) : Entity(tileOrigin, tileBoundingBox) {
	this->tileRow = tileRow;
	this->tileCol = tileCol;
	this->tileType = tileType;
}

bool Tile::isWalkable() const {
	if (tileType == TileType::PELLET || tileType == TileType::EMPTY) return true;
	return false;
}

void Tile::setType(TileType tileType) {
	this->tileType = tileType;
}

TileType Tile::getTileType() const {
	return tileType;
}

Point3D Tile::getCenterPoint() const {
	const auto& absBB = getAbsoluteBoundingBox();
	const auto& min = absBB.min;
	const auto& max = absBB.max;

	float centerX = (min.x + max.x) / 2.0f;
	float centerZ = (min.z + max.z) / 2.0f;

	return Point3D(centerX, 0.0f, centerZ);  // No need for Y
}

float Tile::distanceToCenter(const Tile& other) const {
	Point3D a = getCenterPoint();
	Point3D b = other.getCenterPoint();

	float dx = a.x - b.x;
	float dz = a.z - b.z;

	return std::sqrt(dx * dx + dz * dz);
}

std::string Tile::getTileTypeString() {
	switch (tileType) {
	case TileType::EMPTY:  return "EMPTY";
	case TileType::WALL:   return "WALL";
	case TileType::PELLET: return "PELLET";
	default:               return "NONE";
	}
}

void Tile::renderHighlight() const {
	BoundingBox3D abb = this->getAbsoluteBoundingBox();

	// Set the color for highlighting
	glColor4f(highlightR, highlightG, highlightB, highlightA);

	// Render the plane just above the floor to prevent clipping
	glBegin(GL_QUADS);
	glVertex3f(abb.min.x, abb.min.y + 0.01f, abb.min.z); // Bottom-left
	glVertex3f(abb.max.x, abb.min.y + 0.01f, abb.min.z); // Bottom-right
	glVertex3f(abb.max.x, abb.min.y + 0.01f, abb.max.z); // Top-right
	glVertex3f(abb.min.x, abb.min.y + 0.01f, abb.max.z); // Top-left
	glEnd();
}

void Tile::renderEmpty() const {
	BoundingBox3D abb = this->getAbsoluteBoundingBox();
	glColor3f(0.5f, 0.5f, 0.5f); // Dark gray for floor

	glBegin(GL_QUADS);
	glVertex3f(abb.min.x, abb.min.y, abb.min.z);
	glVertex3f(abb.max.x, abb.min.y, abb.min.z);
	glVertex3f(abb.max.x, abb.min.y, abb.max.z);
	glVertex3f(abb.min.x, abb.min.y, abb.max.z);
	glEnd();
}

void Tile::renderWall() const {
	BoundingBox3D abb = this->getAbsoluteBoundingBox();
	glColor3f(0.3f, 0.3f, 1.0f); // Blue for wall

	float centerX = (abb.min.x + abb.max.x) / 2.0f;
	float centerY = (abb.min.y + abb.max.y) / 2.0f;
	float centerZ = (abb.min.z + abb.max.z) / 2.0f;

	glPushMatrix();
	glTranslatef(centerX, centerY, centerZ);
	glutSolidCube(MapFactory::TILE_SIZE);
	glPopMatrix();
}

void Tile::renderPellet() const {
	BoundingBox3D abb = this->getAbsoluteBoundingBox();
	glColor3f(1.0f, 0.5f, 0.0f); // Orange for pellet

	float centerX = (abb.min.x + abb.max.x) / 2.0f;
	float centerY = abb.min.y + MapFactory::TILE_SIZE / 2.0f;
	float centerZ = (abb.min.z + abb.max.z) / 2.0f;

	glPushMatrix();
	glTranslatef(centerX, centerY, centerZ);
	glutSolidSphere(MapFactory::TILE_SIZE / 8.0, 16, 16);
	glPopMatrix();
}

void Tile::render() const {
	if (highlight) {
		renderHighlight();
	}
	switch (tileType) {
	case TileType::EMPTY:
		renderEmpty();
		break;
	case TileType::WALL:
		renderWall();
		break;
	case TileType::PELLET:
		renderEmpty();
		renderPellet();
		break;
	default:
		return;
	}
}

// Getter and Setter for neighboring tiles
Tile* Tile::getTileUp() const {
	return tileUp;
}

Tile* Tile::getTileDown() const {
	return tileDown;
}

Tile* Tile::getTileLeft() const {
	return tileLeft;
}

Tile* Tile::getTileRight() const {
	return tileRight;
}

int Tile::getTileRow() const {
	return tileRow;
}

int Tile::getTileCol() const {
	return tileCol;
}

void Tile::setTileUp(Tile* tile) {
	tileUp = tile;
}

void Tile::setTileDown(Tile* tile) {
	tileDown = tile;
}

void Tile::setTileLeft(Tile* tile) {
	tileLeft = tile;
}

void Tile::setTileRight(Tile* tile) {
	tileRight = tile;
}

void Tile::setHighlight(bool value) {
	this->highlight = value;
}

void Tile::setHighlightColor(float r, float g, float b, float a) {
	this->highlightR = r;
	this->highlightG = g;
	this->highlightB = b;
	this->highlightA = a;
}


std::string Tile::toString() {
	BoundingBox3D abb = this->getAbsoluteBoundingBox();

	// Calculate the center of the tile
	float textX = (abb.min.x + abb.max.x) / 2.0f;
	float textY = abb.min.y;
	float textZ = (abb.min.z + abb.max.z) / 2.0f;

	// Prepare the coordinate string for output
	std::ostringstream oss;
	oss << "("
		<< std::fixed << std::setprecision(2) << abb.min.x << ","
		<< std::fixed << std::setprecision(2) << abb.min.z
		<< ")";

	std::string coordStr = oss.str();

	// Create the final string with tile coordinates and tile type
	std::ostringstream result;
	result << "Tile at (" << textX << ", " << textY << ", " << textZ
		<< ") - Coordinates: " << coordStr
		<< " - Type: " << this->getTileTypeString();  // Add tileType here

	// Return the generated string
	return result.str();
}

bool Tile::isEqual(const Tile* other) const {
	if (other == nullptr) { return false; }
	if (this->getTileCol() == other->getTileCol() && this->getTileRow() == other->getTileRow()) {
		return true;
	}
	return false;
}

bool Tile::isNeighbor(const Tile* other) const {
	if (other == nullptr) { return false; }
	if (this->isEqual(other)) { return false; }
	if (this->isEqual(other->getTileUp())) { return true; }
	if (this->isEqual(other->getTileDown())) { return true; }
	if (this->isEqual(other->getTileLeft())) { return true; }
	if (this->isEqual(other->getTileRight())) { return true; }
	return false;
}