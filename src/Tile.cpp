#include "Tile.h"
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