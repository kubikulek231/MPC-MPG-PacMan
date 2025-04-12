#include "Tile.h"
#include "MapFactory.h"
#include <cmath>

Tile::Tile(TileType tileType, Point3D tileOrigin, BoundingBox3D tileBoundingBox) : Entity(tileOrigin, tileBoundingBox) {
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